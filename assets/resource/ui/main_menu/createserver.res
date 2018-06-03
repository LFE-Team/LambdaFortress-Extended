"Resource/ui/main_menu/CreateServer.res"
{
	"CTFCreateMultiplayerGame"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFCreateMultiplayerGame"
		"xpos"				"c-200"
		"ypos"				"c-200"
		"wide"				"400"
		"tall"				"400"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"border"				"TFFatLineBorder"
	}
	
	"ColorLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"ColorLabel"
		"xpos"				"10"
		"ypos"				"20"
		"zpos"				"-1"
		"wide"				"600"
		"tall"				"50"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"0"
		"enabled"			"1"
		"labelText"			"SHET"
		"textAlignment"		"west"
		"font"				"HudFontBiggerBold"
		"fgcolor"			"Button.ArmedTextColor"
	}	
	
	"OK"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"OK"
		"xpos"			"200"
		"ypos"			"370"
		"zpos"			"2"
		"wide"			"50"
		"tall"			"20"
		"visible"		"1"
		"enabled"		"1"
		"command"		"Ok"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_OK"
			"textAlignment"		"center"
			"font"				"TallTextSmall"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
	}
	
	"Cancel"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"Cancel"
		"xpos"			"255"
		"ypos"			"370"
		"zpos"			"2"
		"wide"			"60"
		"tall"			"20"
		"visible"		"1"
		"enabled"		"1"
		"command"		"vguicancel"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_Cancel"
			"textAlignment"		"center"
			"font"				"TallTextSmall"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
	}	
	
	"Apply"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"Apply"
		"xpos"			"320"
		"ypos"			"370"
		"zpos"			"2"
		"wide"			"60"
		"tall"			"20"
		"visible"		"1"
		"enabled"		"1"
		"command"		"Apply"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_Apply"
			"textAlignment"		"center"
			"font"				"TallTextSmall"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
	}

	"Defaults"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"Defaults"
		"xpos"				"20"
		"ypos"				"370"
		"wide"				"80"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"Command"			"DefaultsOK"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_UseDefaults"
			"textAlignment"		"center"
			"font"				"TallTextSmall"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
	}
	
	
	"tabbutton"
	{
		"ControlName"		"CAdvTabs"
		"fieldName"			"tabbutton"
		"xpos"				"25"
		"ypos"				"20"
		"zpos"				"6"
		"wide"				"350"
		"tall"				"25"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"		
		
		"ServerButton"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"ServerButton"
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"			"1"		
			"command"				"newcreategameservertab"
			
			"SubButton"
			{
				"labelText"			"#GameUI_Multiplayer"
				"xshift" 			"0"
				"yshift"			"-3"
				"stayselectedonclick"	"1"
				"textAlignment"		"center"	
				"font"				"TallTextSmall"
				"border_default"		"AdvLeftButtonDefault"
				"border_armed"			"AdvLeftButtonArmed"
				"border_depressed"		"AdvLeftButtonDepressed"
				"selectedFgColor_override"		"255 255 180 255"
			}
		}
	}
}