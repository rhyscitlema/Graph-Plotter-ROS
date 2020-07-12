

printVBEModeInfo:
	push	eax
	push	esi
	call	newline

	mov	esi, .strVBEVersion
	call	printstr
	mov	ax, [VBE_VERSION]
	call	printhex2
	call	newline

	mov	esi, .strVBEModeNumber
	call	printstr
	mov	ax, VBEMODE
	call	printhex2
	call	newline

	call	newline

	mov	esi, .strBytesPerScanLine
	call	printstr
	mov	ax, [vbeModeInfo_BytesPerScanLine]
	call	printhex2
	call	newline

	mov	esi, .strXResolution
	call	printstr
	mov	ax, [vbeModeInfo_XResolution]
	call	printhex2
	call	newline

	mov	esi, .strYResolution
	call	printstr
	mov	ax, [vbeModeInfo_YResolution]
	call	printhex2
	call	newline

	mov	esi, .strXCharSize
	call	printstr
	mov	ax, [vbeModeInfo_XCharSize]
	call	printhex1
	call	newline

	mov	esi, .strYCharSize
	call	printstr
	mov	ax, [vbeModeInfo_YCharSize]
	call	printhex1
	call	newline

	mov	esi, .strNumberOfPlanes
	call	printstr
	mov	ax, [vbeModeInfo_NumberOfPlanes]
	call	printhex1
	call	newline

	mov	esi, .strBitsPerPixel
	call	printstr
	mov	ax, [vbeModeInfo_BitsPerPixel]
	call	printhex1
	call	newline

	call	newline

	mov	esi, .strRedMaskSize
	call	printstr
	mov	ax, [vbeModeInfo_RedMaskSize]
	call	printhex1
	call	newline

	mov	esi, .strRedFieldPosition
	call	printstr
	mov	ax, [vbeModeInfo_RedFieldPosition]
	call	printhex1
	call	newline

	mov	esi, .strGreenMaskSize
	call	printstr
	mov	ax, [vbeModeInfo_GreenMaskSize]
	call	printhex1
	call	newline

	mov	esi, .strGreenFieldPosition
	call	printstr
	mov	ax, [vbeModeInfo_GreenFieldPosition]
	call	printhex1
	call	newline

	mov	esi, .strBlueMaskSize
	call	printstr
	mov	ax, [vbeModeInfo_BlueMaskSize]
	call	printhex1
	call	newline

	mov	esi, .strBlueFieldPosition
	call	printstr
	mov	ax, [vbeModeInfo_BlueFieldPosition]
	call	printhex1
	call	newline

	mov	esi, .strRsvdMaskSize
	call	printstr
	mov	ax, [vbeModeInfo_RsvdMaskSize]
	call	printhex1
	call	newline

	mov	esi, .strRsvdFieldPosition
	call	printstr
	mov	ax, [vbeModeInfo_RsvdFieldPosition]
	call	printhex1
	call	newline

	mov	esi, .strDirectColorModeInfo
	call	printstr
	mov	ax, [vbeModeInfo_DirectColorModeInfo]
	call	printhex1
	call	newline

	call	newline

	mov	esi, .strPhysBasePtr
	call	printstr
	mov	eax, [vbeModeInfo_PhysBasePtr]
	call	printhex4
	call	newline

	pop	esi
	pop	eax
	ret

.strVBEVersion		db "   VBE Version              =     ", 0
.strVBEModeNumber:	db "   VBE Mode Number          =     ", 0

.strBytesPerScanLine:	db "   Bytes Per Scan Line      =     ", 0
.strXResolution:	db "   X Resolution             =     ", 0
.strYResolution:	db "   Y Resolution             =     ", 0
.strXCharSize:		db "   X Character Size         =     ", 0
.strYCharSize:		db "   Y Character Size         =     ", 0
.strNumberOfPlanes:	db "   Number of Planes         =     ", 0
.strBitsPerPixel	db "   Bits Per Pixel           =     ", 0

.strRedMaskSize		db "   Red Mask Size            =     ", 0
.strRedFieldPosition	db "   Red Field Position       =     ", 0
.strGreenMaskSize	db "   Green Mask Size          =     ", 0
.strGreenFieldPosition	db "   Green Field Position     =     ", 0
.strBlueMaskSize	db "   Blue Mask Size           =     ", 0
.strBlueFieldPosition	db "   Blue Field Position      =     ", 0
.strRsvdMaskSize	db "   Reserved Mask Size       =     ", 0
.strRsvdFieldPosition	db "   Reserved Field Position  =     ", 0
.strDirectColorModeInfo	db "   Direct Color Mode Info   =     ", 0

.strPhysBasePtr		db "   Physical Base Pointer    =     ", 0
