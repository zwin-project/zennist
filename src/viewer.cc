#include "viewer.h"

#include <sys/mman.h>
#include <unistd.h>

#include "gltf.vert.h"
#include "gltf_color.frag.h"
#include "gltf_texture.frag.h"
#include "texture-factory.h"

namespace zennist {

Viewer::Viewer(zukou::System *system, zukou::VirtualObject *virtual_object)
    : system_(system),
      virtual_object_(virtual_object),
      pool_(system),
      vertex_shader_(system),
      texture_fragment_shader_(system),
      color_fragment_shader_(system),
      sampler_(system)
{}

Viewer::~Viewer()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

bool
Viewer::Load(const char *model_path)
{
  model_ = new tinygltf::Model();
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;
  std::string path(model_path);

  bool ret = false;
  ret = loader.LoadASCIIFromFile(model_, &err, &warn, path.c_str());

  if (!warn.empty()) std::cerr << warn << std::endl;
  if (!err.empty()) {
    std::cerr << err << std::endl;
    return false;
  }

  if (!ret) {
    std::cerr << "Failed to load .glTF : " << path << std::endl;
    return false;
  }

  parent_dir_ = std::filesystem::absolute(path).parent_path();

  return true;
}

bool
Viewer::Render(float radius, glm::mat4 transform, const char *model_path)
{
  this->SetInitialPosition(radius, transform);

  if (!this->Load(model_path)) {
    std::cerr << "Failed to load model: " << model_path << std::endl;
    return false;
  }

  if (!this->Setup()) {
    std::cerr << "Failed to setup buffer" << std::endl;
    return false;
  }

  if (!this->RenderScene()) {
    std::cerr << "Failed to render scene" << std::endl;
    return false;
  }

  return true;
}

bool
Viewer::Setup()
{
  if (model_->buffers.size() == 0) {
    std::cerr << "buffer size is zero." << std::endl;
    return false;
  }
  if (model_->buffers.size() > 1) {
    std::cerr << "TODO: support buffer size is more than one." << std::endl;
    return false;
  }

  assert(model_->buffers.size() == 1);
  size_t pool_size = model_->buffers.at(0).data.size();

  fd_ = zukou::Util::CreateAnonymousFile(pool_size);

  if (!pool_.Init(fd_, pool_size)) return false;

  {
    auto buffer_data = static_cast<char *>(
        mmap(nullptr, pool_size, PROT_WRITE, MAP_SHARED, fd_, 0));
    std::memcpy(buffer_data, model_->buffers.at(0).data.data(), pool_size);
    munmap(buffer_data, pool_size);
  }

  if (!vertex_shader_.Init(GL_VERTEX_SHADER, gltf_vert_shader_source))
    return false;
  if (!texture_fragment_shader_.Init(
          GL_FRAGMENT_SHADER, gltf_texture_frag_shader_source))
    return false;
  if (!color_fragment_shader_.Init(
          GL_FRAGMENT_SHADER, gltf_color_frag_shader_source))
    return false;

  program_map_["texture"] = new zukou::GlProgram(system_);
  if (!program_map_["texture"]->Init()) {
    return false;
  }
  program_map_["texture"]->AttachShader(&vertex_shader_);
  program_map_["texture"]->AttachShader(&texture_fragment_shader_);
  program_map_["texture"]->Link();

  program_map_["color"] = new zukou::GlProgram(system_);
  if (!program_map_["color"]->Init()) {
    return false;
  }
  program_map_["color"]->AttachShader(&vertex_shader_);
  program_map_["color"]->AttachShader(&color_fragment_shader_);
  program_map_["color"]->Link();

  if (!sampler_.Init()) return false;
  sampler_.Parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  sampler_.Parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  for (int i = 0; i < (int)model_->bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model_->bufferViews[i];
    if (bufferView.target == 0) {
      std::cout << "TODO: bufferView.target is zero (unsupported)" << std::endl;
      continue;
    }

    int sparse_accessor = -1;
    for (int a_i = 0; a_i < (int)model_->accessors.size(); ++a_i) {
      const auto &accessor = model_->accessors[a_i];
      if (accessor.bufferView == i) {
        if (accessor.sparse.isSparse) {
          std::cout << "TODO: support sparse_accessor" << std::endl;
          sparse_accessor = a_i;
          break;
        }
      }
    }

    if (sparse_accessor >= 0) {
      std::cout << "TODO: support sparse_accessor" << std::endl;
      continue;
    }

    zukou::Buffer *zBuffer = new zukou::Buffer();
    if (!zBuffer->Init(&pool_, bufferView.byteOffset, bufferView.byteLength)) {
      std::cerr << "Failed to initialize vertex buffer" << std::endl;
      return false;
    }

    zukou::GlBuffer *zGlBuffer = new zukou::GlBuffer(system_);
    gl_vertex_buffer_map_[i] = zGlBuffer;
    if (!gl_vertex_buffer_map_[i]->Init()) {
      std::cerr << "Failed to initialize gl vertex buffer" << std::endl;
      return false;
    }
    gl_vertex_buffer_map_[i]->Data(bufferView.target, zBuffer, GL_STATIC_DRAW);
  }

  for (auto texture : model_->textures) {
    assert((int)model_->images.size() > texture.source);
    auto image = model_->images[texture.source];

    std::filesystem::path path = parent_dir_;
    path /= image.uri;

    Texture *texture_instance = TextureFactory::Create(system_, path.c_str());
    if (texture_instance == nullptr) {
      std::cerr << "Failed to create texture: " << path << std::endl;
      return false;
    }
    if (!texture_instance->Init()) {
      std::cerr << "Failed to initialize texture: " << path << std::endl;
      return false;
    }
    if (!texture_instance->Load(path.c_str())) {
      std::cerr << "Failed to load texture: " << path << std::endl;
      return false;
    }
    texture_map_.emplace(texture.source, texture_instance);
  }

  return true;
}

bool
Viewer::RenderScene()
{
  assert(model_->scenes.size() > 0);

  int scene_to_display = model_->defaultScene > -1 ? model_->defaultScene : 0;
  const tinygltf::Scene &scene = model_->scenes[scene_to_display];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    if (!this->RenderNode(model_->nodes[scene.nodes[i]])) return false;
  }

  return true;
}

bool
Viewer::RenderNode(const tinygltf::Node &node)
{
  if (node.matrix.size() == 16) {
    // TODO: row major or column major
    matrix_stack_.push_back(glm::make_mat4(node.matrix.data()));
  } else {
    glm::mat4 mat(1);

    if (node.translation.size() == 3) {
      glm::mat4 T = glm::translate(
          glm::mat4(1), glm::vec3(node.translation[0], node.translation[1],
                            node.translation[2]));
      mat *= T;
    }

    if (node.rotation.size() == 4) {
      glm::mat4 R = glm::toMat4(glm::quat(node.rotation[3], node.rotation[0],
          node.rotation[1], node.rotation[2]));
      mat *= R;
    }

    if (node.scale.size() == 3) {
      glm::mat4 S = glm::scale(
          glm::mat4(1), glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
      mat *= S;
    }

    matrix_stack_.push_back(mat);
  }

  if (node.mesh > -1) {
    assert(node.mesh < (int)model_->meshes.size());
    if (!this->RenderMesh(model_->meshes[node.mesh])) return false;
  }

  for (size_t i = 0; i < node.children.size(); i++) {
    assert(node.children[i] < (int)model_->nodes.size());
    if (!this->RenderNode(model_->nodes[node.children[i]])) return false;
  }

  matrix_stack_.pop_back();

  return true;
}

bool
Viewer::RenderMesh(const tinygltf::Mesh &mesh)
{
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    zukou::RenderingUnit *rendering_unit = new zukou::RenderingUnit(system_);
    zukou::GlBaseTechnique *base_technique =
        new zukou::GlBaseTechnique(system_);

    if (!rendering_unit->Init(virtual_object_)) {
      std::cerr << "Failed to initialize rendering_unit" << std::endl;
      return false;
    }

    if (!base_technique->Init(rendering_unit)) {
      std::cerr << "Failed to initialize base_technique" << std::endl;
      return false;
    }

    zukou::GlVertexArray *vertex_array = new zukou::GlVertexArray(system_);
    if (!vertex_array->Init()) {
      std::cerr << "Failed to initialize vertex_array" << std::endl;
      return false;
    }

    const tinygltf::Primitive &primitive = mesh.primitives[i];

    assert((int)model_->materials.size() > primitive.material);
    if (primitive.material < 0) continue;  // FIXME:

    tinygltf::Material material = model_->materials[primitive.material];

    int texture_index = material.pbrMetallicRoughness.baseColorTexture.index;
    // TODO: sampler setting
    if (texture_index >= 0) {
      tinygltf::Texture texture = model_->textures[texture_index];
      auto gl_texture = texture_map_[texture.source];
      gl_texture->GenerateMipmap(GL_TEXTURE_2D);
      base_technique->Bind(
          0, "in_texture", gl_texture, GL_TEXTURE_2D, &sampler_);

      for (auto [extension, value] :
          material.pbrMetallicRoughness.baseColorTexture.extensions) {
        if (extension == "KHR_texture_transform") {
          glm::vec2 uniform_offset(0.0f, 0.0f);
          if (value.Has("offset")) {
            auto offset = value.Get("offset");
            uniform_offset[0] = offset.Get(0).GetNumberAsDouble();
            uniform_offset[1] = offset.Get(1).GetNumberAsDouble();
          }
          base_technique->Uniform(0, "in_offset", uniform_offset);

          glm::vec2 uniform_scale(1.0f, 1.0f);
          if (value.Has("scale")) {
            auto scale = value.Get("scale");
            uniform_scale[0] = scale.Get(0).GetNumberAsDouble();
            uniform_scale[1] = scale.Get(1).GetNumberAsDouble();
          }
          base_technique->Uniform(0, "in_scale", uniform_scale);

          glm::vec1 uniform_rotation(0.0f);
          if (value.Has("rotation")) {
            auto rotation = value.Get("rotation");
            uniform_rotation[0] = rotation.GetNumberAsDouble();
          }
          base_technique->Uniform(0, "in_rotation", uniform_rotation);
        }
      }

      glm::vec1 uniform_alpha_cutoff(0.0f);
      if (material.alphaMode == "MASK") {
        uniform_alpha_cutoff[0] = material.alphaCutoff;
      }
      base_technique->Uniform(0, "in_alpha_cutoff", uniform_alpha_cutoff);

      base_technique->Bind(program_map_["texture"]);
    } else {
      assert(material.pbrMetallicRoughness.baseColorFactor.size() == 4);

      auto base_color = material.pbrMetallicRoughness.baseColorFactor;

      base_technique->Uniform(0, "in_base_color",
          glm::vec4(
              base_color[0], base_color[1], base_color[2], base_color[3]));
      base_technique->Bind(program_map_["color"]);
    }

    for (auto [attribute, index] : primitive.attributes) {
      assert(index >= 0);
      const tinygltf::Accessor &accessor = model_->accessors[index];
      int size = 1;
      if (accessor.type == TINYGLTF_TYPE_SCALAR) {
        size = 1;
      } else if (accessor.type == TINYGLTF_TYPE_VEC2) {
        size = 2;
      } else if (accessor.type == TINYGLTF_TYPE_VEC3) {
        size = 3;
      } else if (accessor.type == TINYGLTF_TYPE_VEC4) {
        size = 4;
      } else {
        assert(0);
      }

      if ((attribute != "POSITION") && (attribute != "NORMAL") &&
          (attribute != "TEXCOORD_0")) {
        std::cerr << "TODO: support attribute name is " << attribute
                  << std::endl;
        continue;
      }

      // TODO: more robust
      int location;
      if (attribute == "POSITION")
        location = 0;
      else if (attribute == "NORMAL")
        location = 1;
      else if (attribute == "TEXCOORD_0")
        location = 2;
      else
        assert(0);

      // compute byteStride from accessor + bufferView.
      int byteStride =
          accessor.ByteStride(model_->bufferViews[accessor.bufferView]);
      assert(byteStride != -1);

      assert(gl_vertex_buffer_map_.count(accessor.bufferView) > 0);
      vertex_array->Enable(location);
      vertex_array->VertexAttribPointer(location, size, accessor.componentType,
          accessor.normalized ? GL_TRUE : GL_FALSE, byteStride,
          accessor.byteOffset, gl_vertex_buffer_map_[accessor.bufferView]);
    }
    base_technique->Bind(vertex_array);

    assert(primitive.indices >= 0);
    const tinygltf::Accessor &indexAccessor =
        model_->accessors[primitive.indices];

    int mode = -1;
    switch (primitive.mode) {
      case TINYGLTF_MODE_TRIANGLES:
        mode = GL_TRIANGLES;
        break;
      case TINYGLTF_MODE_TRIANGLE_STRIP:
        mode = GL_TRIANGLE_STRIP;
        break;
      case TINYGLTF_MODE_TRIANGLE_FAN:
        mode = GL_TRIANGLE_FAN;
        break;
      case TINYGLTF_MODE_POINTS:
        mode = GL_POINTS;
        break;
      case TINYGLTF_MODE_LINE:
        mode = GL_LINES;
        break;
      case TINYGLTF_MODE_LINE_LOOP:
        mode = GL_LINE_LOOP;
        break;
      default:
        assert(0);
    }

    if (model_->bufferViews[indexAccessor.bufferView].target !=
        GL_ELEMENT_ARRAY_BUFFER) {
      std::cerr << "TODO: support other than gl_element_array_buffer"
                << std::endl;
    }

    base_technique->Uniform(0, "local_model", CalculateLocalModel());

    base_technique->DrawElements(mode, indexAccessor.count,
        indexAccessor.componentType, indexAccessor.byteOffset,
        gl_vertex_buffer_map_[indexAccessor.bufferView]);
  }

  return true;
}

void
Viewer::SetInitialPosition(float radius, glm::mat4 transform)
{
  matrix_stack_.push_back(glm::scale(transform, glm::vec3(radius)));
}

glm::mat4
Viewer::CalculateLocalModel()
{
  glm::mat4 value(1);
  for (auto matrix : matrix_stack_) {
    value *= matrix;
  }
  return value;
}

}  // namespace zennist
