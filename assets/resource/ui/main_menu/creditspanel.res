"Resource/UI/main_menu/CreditsPanel.res"
{
	"CTFCreditsPanel"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"CTFCreditsPanel"
		"xpos"			"c-250"
		"ypos"			"-2"
		"wide"			"500"
		"tall"			"510"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"border"		"AdvSquareButtonDefault"
	}
	"TitleLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"TitleLabel"
		"xpos"				"20"
		"ypos"				"10"
		"zpos"				"5"
		"wide"				"600"
		"tall"				"25"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"Credits"
		"textAlignment"		"west"
		"font"				"HudFontBiggerBold"
		"fgcolor"			"AdvTextDefault"
	}	
	
	"TitleLabelShadow"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"TitleLabelShadow"
		"xpos"				"22"
		"ypos"				"12"
		"zpos"				"4"
		"wide"				"600"
		"tall"				"25"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"Credits"
		"textAlignment"		"west"
		"font"				"HudFontBiggerBold"
		"fgcolor"			"TransparentBlack"
	}	

	"FadeTransition"
	{
		"ControlName"			"EditablePanel"
		"fieldName"				"FadeTransition"
		"xpos"					"10"
		"ypos"					"40"
		"zpos"					"4"
		"wide"					"480"
		"tall"					"380"
		"visible"				"1"
		"enabled"				"1"
		"scaleImage"			"1"
		"mouseinputenabled"		"0"
		
		"bgcolor_override"		"26 23 22 100"
	}

	"CancelButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"CancelButton"
		"xpos"				"200"
		"ypos"				"430"
		"zpos"				"6"
		"wide"				"124"
		"tall"				"25"
		"visible"			"1"
		"enabled"			"1"
		"command"			"vguicancel"	
		
		"SubButton"
		{
			"labelText" 		"Back"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}		
	}

	"CreditText"
	{
		"ControlName"		"CExRichText"
		"fieldName"			"CreditText"
		"font"				"HudFontSmallBold"
		"xpos"				"10"
		"ypos"				"40"
		"zpos"				"5"
		"wide"				"480"
		"tall"				"380"
		"autoResize"		"3"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"paintborder"		"0"
		"textAlignment"		"northwest"
		"fgcolor"			"Grey"
		"textfile"			"credits.txt"
		"scroll_box_image"	"main_menu/button_square"
	}
}
		