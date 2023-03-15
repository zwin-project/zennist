if get_option('wallpapers')
  wallpapers = files(
    'Zennist_Wallpaper_Gradient_3840x2160.png',
    'Zennist_Wallpaper_Main_3840x2160.png',
    'Zennist_Wallpaper_Dark_3840x2160.png',
    'Zennist_Wallpaper_Light_3840x2160.png',
    'Zennist_Wallpaper_Mosaic_3840x2160.png',
  )

  wallpaper_install_dir = get_option('datadir') / 'backgrounds' / 'zennist'

  install_data(
    wallpapers,
    install_dir: wallpaper_install_dir,
  )
endif
