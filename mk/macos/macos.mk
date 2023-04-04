CC               := clang

NASMFLAGS        := -f macho64 --prefix "_"

LIBS             := -F /Library/Frameworks -framework SDL2 -framework OpenGL
CFLAGS           := -I /Library/Frameworks/SDL2.framework/Headers -DGL_SILENCE_DEPRECATION
OSX_APP          := releases/macos/zzterm-$(VERSION_NAME).app

.PHONY: release.macos
release: release.macos

release.macos: releases/macos/zzterm-macos-$(VERSION_NAME).tgz
releases/macos/zzterm-macos-$(VERSION_NAME).tgz: $(OSX_APP)/Contents/Info.plist
releases/macos/zzterm-macos-$(VERSION_NAME).tgz: $(OSX_APP)/Contents/MacOS/zzterm
releases/macos/zzterm-macos-$(VERSION_NAME).tgz: $(OSX_APP)/Contents/Frameworks/SDL2.framework
releases/macos/zzterm-macos-$(VERSION_NAME).tgz: $(OSX_APP)/Contents/Resources/zzterm.icns
	cd releases/macos/ && tar -czf zzterm-macos-$(VERSION_NAME).tgz zzterm-$(VERSION_NAME).app

$(OSX_APP)/Contents/MacOS/zzterm: $(SOURCES)
	mkdir -p $(OSX_APP)/Contents/MacOS
	$(CC) $^ -o $@ $(RELEASE_OPTIMIZATION) $(CFLAGS) $(CINCLUDES) $(WARNINGS) $(LIBS)
	strip -SxX $@
	install_name_tool -change "@rpath/SDL2.framework/Versions/A/SDL2" "@executable_path/../Frameworks/SDL2.framework/SDL2" $@

$(OSX_APP)/Contents/Info.plist:  mk/macos/Info.plist
	mkdir -p $(OSX_APP)/Contents
	cp -f mk/macos/Info.plist $(OSX_APP)/Contents/Info.plist

$(OSX_APP)/Contents/Frameworks/SDL2.framework: ~/ReleaseFrameworks/SDL2.framework
	mkdir -p $(OSX_APP)/Contents/Frameworks/
	rsync -a $< $(OSX_APP)/Contents/Frameworks/

$(OSX_APP)/Contents/Resources/zzterm.icns: releases/macos/zzterm.iconset/icon_16x16.png
$(OSX_APP)/Contents/Resources/zzterm.icns: releases/macos/zzterm.iconset/icon_16x16@2x.png
$(OSX_APP)/Contents/Resources/zzterm.icns: releases/macos/zzterm.iconset/icon_32x32.png
$(OSX_APP)/Contents/Resources/zzterm.icns: releases/macos/zzterm.iconset/icon_32x32@2x.png
$(OSX_APP)/Contents/Resources/zzterm.icns: releases/macos/zzterm.iconset/icon_64x64.png
$(OSX_APP)/Contents/Resources/zzterm.icns: releases/macos/zzterm.iconset/icon_64x64@2x.png
$(OSX_APP)/Contents/Resources/zzterm.icns: releases/macos/zzterm.iconset/icon_128x128.png
$(OSX_APP)/Contents/Resources/zzterm.icns: releases/macos/zzterm.iconset/icon_128x128@2x.png
$(OSX_APP)/Contents/Resources/zzterm.icns: releases/macos/zzterm.iconset/icon_256x256.png
	mkdir -p $(OSX_APP)/Contents/Resources/
	iconutil -c icns -o $@ releases/macos/zzterm.iconset

releases/macos/zzterm.iconset/icon_16x16.png: logo.png
	mkdir -p releases/macos/zzterm.iconset/
	convert -resize "16x16" $< $@
releases/macos/zzterm.iconset/icon_32x32.png: logo.png
	mkdir -p releases/macos/zzterm.iconset/
	convert -resize "32x32" $< $@
releases/macos/zzterm.iconset/icon_16x16@2x.png: logo.png
	mkdir -p releases/macos/zzterm.iconset/
	convert -resize "32x32" $< $@
releases/macos/zzterm.iconset/icon_32x32@2x.png: logo.png
	mkdir -p releases/macos/zzterm.iconset/
	convert -resize "64x64" $< $@
releases/macos/zzterm.iconset/icon_64x64.png: logo.png
	mkdir -p releases/macos/zzterm.iconset/
	convert -resize "64x64" $< $@
releases/macos/zzterm.iconset/icon_64x64@2x.png: logo.png
	mkdir -p releases/macos/zzterm.iconset/
	convert -resize "128x128" $< $@
releases/macos/zzterm.iconset/icon_128x128.png: logo.png
	mkdir -p releases/macos/zzterm.iconset/
	convert -resize "128x128" $< $@
releases/macos/zzterm.iconset/icon_128x128@2x.png: logo.png
	mkdir -p releases/macos/zzterm.iconset/
	convert -resize "256x256" $< $@
releases/macos/zzterm.iconset/icon_256x256.png: logo.png
	mkdir -p releases/macos/zzterm.iconset/
	convert -resize "256x256" $< $@
