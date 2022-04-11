all:
	make -C libavutil
	make -C libavformat
	make -C libavcodec
	rm -f pmpmod/PMPMOD.elf
	rm -f pmpmod/PARAM.SFO
	rm -f pmpmod/EBOOT.PBP
	make -C pmpmod
	make -C pmpmod kxploit
	pack-pbp pmpmod/PMPMOD_M4g3%/EBOOT.PBP pmpmod/PARAM.SFO pmpmod/pack-pbp/ICON0.PNG NULL NULL pmpmod/pack-pbp/PIC1.PNG NULL NULL NULL
	cp pmp.ini pmpmod/PMPMOD_M4g3/
	cp skin.tga pmpmod/PMPMOD_M4g3/ 
	cp interf.tga pmpmod/PMPMOD_M4g3/
clean:
	make -C libavutil clean
	make -C libavformat clean
	make -C libavcodec clean
	make -C pmpmod clean
	rm -f -r pmpmod/PMPMOD_M4g3
	rm -f -r pmpmod/PMPMOD_M4g3%
upload: all
	mount /mnt/psp && cp -v pmpmod/PMPMOD_M4g3.elf /mnt/psp/apps/p.elf && sync && umount /mnt/psp
	
	
	
