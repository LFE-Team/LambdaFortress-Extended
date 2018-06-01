"Resource/UI/LoadingProgress.res"
{
	"TFLoadingProgress"
	{
		"ControlName"	"EditablePanel"
		"fieldName"	"TFLoadingProgress"
		"xpos"		"0"
		"ypos"		"0"
		"wide"		"f0"
		"tall"		"480"
		"visible"	"1"
		"enabled"	"1"
	}
	"MainBackground"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"MainBackground"
		"xpos"			"0"
		"ypos"			"0"
		"wide"			"f0"
		"tall"			"480"
		"visible"		"1"
		"enabled"		"1"
		"image"			""
		"scaleImage"		"1"
	}
	"MapBackground"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"MapBackground"
		"xpos"			"0"
		"ypos"			"0"
		"wide"			"f0"
		"tall"			"480"
		"visible"		"0"
		"enabled"		"1"
		"image"			""
		"scaleImage"	"1"		
	}
	"OnYourWayLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"OnYourWayLabel"
		"font"			"HudFontMediumBold"
		"labelText"		"#LoadingMap"
		"textAlignment"		"center"
		"xpos"			"c0"
		"ypos"			"50"
		"zpos"			"50"
		"wide"			"360"
		"tall"			"35"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}
	"OnYourWayLabelShadow"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"OnYourWayLabelShadow"
		"font"			"HudFontMediumBold"
		"labelText"		"#LoadingMap"
		"textAlignment"		"center"
		"xpos"			"c2"
		"ypos"			"52"
		"zpos"			"50"
		"wide"			"360"
		"tall"			"35"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"fgcolor"		"TransparentBlack"
	}
	"MapLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"MapLabel"
		"font"			"HudFontMediumBigBold"
		"labelText"		"%maplabel%"
		"textAlignment"		"center"
		"xpos"			"c5" 
		"ypos"			"95"
		"ypos_hidef"		"425"
		"ypos_lodef"		"425"
		"zpos"			"50"
		"wide"			"350"
		"tall"			"35"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}
	"MapLabelShadow"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"MapLabelShadow"
		"font"			"HudFontMediumBigBold"
		"labelText"		"%maplabel%"
		"textAlignment"		"center"
		"xpos"			"c7" 
		"ypos"			"97"
		"ypos_hidef"		"425"
		"ypos_lodef"		"425"
		"zpos"			"50"
		"wide"			"350"
		"tall"			"35"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"fgcolor"		"TransparentBlack"
	}
	"TipLabel"
	{
		"ControlName"		"Label"
		"fieldName"		"TipLabel"
		"font"			"HudFontSmallBold"
		"font_hidef"	"HudFontSmallBold"
		"font_lodef"	"HudFontSmallBold"
		"labelText"		"#StatSummary_Label_TIP"
		"textAlignment"		"west"
		"textAlignment_hidef"	"east"
		"textAlignment_lodef"	"east"
		"xpos"			"c-285"
		"xpos_hidef"			"c-295"
		"xpos_lodef"			"c-295"
		"ypos"			"420"
		"ypos_hidef"			"362"
		"ypos_lodef"			"362"
		"zpos"			"12"
		"zpos_hidef"	"50"
		"zpos_lodef"	"50"
		"wide"			"200"
		"wide_hidef"		"100"
		"wide_lodef"		"100"
		"tall"			"25"
		"tall_hidef"	"24"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"paintborder"	"1"
	}
	"TipText"
	{
		"ControlName"		"Label"
		"fieldName"		"TipText"
		"font"			"ChalkboardText"
		"font_hidef"	"ChalkboardText"
		"labelText"		"%tiptext%"
		"textAlignment"			"west"
		"textAlignment_hidef"		"north-west"
		"textAlignment_lodef"		"north-west"
		"xpos"			"c-275"
		"xpos_lodef"			"c-180"
		"xpos_hidef"			"c-180"
		"ypos"			"405"
		"ypos_hidef"			"362"
		"ypos_lodef"			"362"
		"zpos"			"12"
		"wide"			"350"
		"wide_hidef"		"360"
		"wide_lodef"		"360"
		"tall"			"48"
		"tall_hidef"	"60"
		"tall_lodef"	"60"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"wrap"			"1"
		"paintborder"	"1"
	}
	"NextTipButton" [$WIN32]
	{
		"ControlName"		"Button"
		"fieldName"		"NextTipButton"
		"xpos"			"c-248"
		"ypos"			"450"
		"zpos"			"6"
		"wide"			"90"
		"tall"			"15"
		"autoResize"	"0"
		"pinCorner"		"2"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"
		"labelText"		"#TF_NextTip"
		"textAlignment"	"center"
		"dulltext"		"0"
		"brighttext"	"0"
		"Command"		"nexttip"
	}
	"CloseButton" [$WIN32]
	{
		"ControlName"		"Button"
		"fieldName"		"CloseButton"
		"xpos"			"r190"
		"ypos"			"r50"
		"zpos"			"6"
		"wide"			"150"
		"tall"			"30"
		"autoResize"	"0"
		"pinCorner"		"2"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"
		"labelText"		"#TF_Close"
		"textAlignment"	"center"
		"dulltext"		"0"
		"brighttext"	"0"
		"Command"		"vguicancel"
	}
	"TitleBanner"
	{
		"ControlName"	"ImagePanel"
		"xpos"			"0"
		"ypos"			"0"
		"wide"			"f0"
		"tall"			"128"
		"image"			"menu_header"
		"visible_lodef"	"0"
		"enabled_lodef"	"0"
	}

	"Footer"
	{
		"ControlName"		"EditablePanel"
		"fieldName"		"Footer"
		"xpos"			"0"
		"ypos"			"r60"
		"wide"			"f0"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"textAlignment"		"west"
		"dulltext"		"0"
		"brighttext"		"1"
		"bgcolor"			"TanDarker"
		"paintbackground"	"1"
		"tall"				"100"
		"tall_lodef"				"60"
		"center"			"1"
		"zpos"				"6"
		"fgcolor"			"HudOffWhite" 	
	}
}