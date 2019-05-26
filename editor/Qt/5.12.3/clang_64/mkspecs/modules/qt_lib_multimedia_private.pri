QT.multimedia_private.VERSION = 5.12.3
QT.multimedia_private.name = QtMultimedia
QT.multimedia_private.module =
QT.multimedia_private.libs = $$QT_MODULE_LIB_BASE
QT.multimedia_private.includes = $$QT_MODULE_LIB_BASE/QtMultimedia.framework/Headers/5.12.3 $$QT_MODULE_LIB_BASE/QtMultimedia.framework/Headers/5.12.3/QtMultimedia
QT.multimedia_private.frameworks =
QT.multimedia_private.depends = core_private gui_private multimedia
QT.multimedia_private.uses =
QT.multimedia_private.module_config = v2 lib_bundle internal_module
QT.multimedia_private.enabled_features = avfoundation openal
QT.multimedia_private.disabled_features = alsa directshow directshow-player evr gpu_vivante gstreamer_1_0 gstreamer_0_10 gstreamer gstreamer_app gstreamer_encodingprofiles gstreamer_photography linux_v4l pulseaudio resourcepolicy wasapi wmf wmf-player wmsdk wshellitem
QMAKE_LIBS_AVFOUNDATION = -framework AVFoundation -framework Foundation
QMAKE_LIBS_OPENAL = -framework OpenAL
