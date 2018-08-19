"Resource/UI/main_menu/BlogPanel.res"
{	
	"BlogPanel"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"BlogPanel"
		"xpos"				"r500"
		"ypos"				"70"
		"zpos"				"3"
		"wide"				"320"
		"tall"				"330"
		"visible"			"1"
		"enabled"			"1"
	}	
	
	"WelcomeBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"WelcomeBG"
		"xpos"				"0"
		"ypos"				"5"
		"zpos"				"4"
		"wide"				"300"
		"tall"				"30"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuBGBorder"
	}
	
	"DevLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"DevLabel"
		"xpos"				"15"
		"ypos"				"10"
		"zpos"				"6"
		"wide"				"200"
		"tall"				"20"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"alpha"				"255"
		"labelText"			"Latest Changelog"
		"textAlignment"		"west"
		"font"				"HudFontSmallBoldShadow"
		"fgcolor"			"AdvTextDefault"
	}
	
	"HTMLPanel"
	{
		"ControlName"		"HTML"
		"fieldName"			"HTMLPanel"
		"xpos"				"5"
		"ypos"				"30"
		"zpos"				"1"
		"wide"				"390"
		"tall"				"310"
		"visible"			"0"
		"enabled"			"0"
		"font"				"MenuMainTitle"
	}

	
	"InfoText"
	{
		"ControlName"		"CExRichText"
		"fieldName"			"InfoText"
		"font"				"TeamMenu"
		"xpos"				"5"
		"ypos"				"160"
		"zpos"				"5"
		"wide"				"310"
		"tall"				"165"
		"autoResize"		"3"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"paintborder"		"0"
		"textAlignment"		"northwest"
		"fgcolor"			"Black"
		"textfile"			"changelog.txt"
	}

	"BlogBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"BlogBG"
		"xpos"				"5"
		"ypos"				"30"
		"zpos"				"3"
		"wide"				"291"
		"tall"				"310"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuHighlightBorder"
	}

	"BlogImageBG"
	{
		"ControlName"	"ScalableImagePanel"
		"fieldName"		"BlogImageBG"
		"xpos"			"15"
		"ypos"			"45"
		"zpos"			"4"
		"wide"			"270"
		"tall"			"110"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../console/background05"
		"tileImage"		"1"
	}

	"BlogImage"
	{
		"ControlName"	"ScalableImagePanel"
		"fieldName"		"BlogImage"
		"xpos"			"85"
		"ypos"			"30"
		"zpos"			"5"
		"wide"			"140"
		"tall"			"140"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../backpack/weapons/gunslinger_large"
	}
}		