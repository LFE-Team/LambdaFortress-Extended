"Resource/ui/main_menu/OptionsDialog.res"
{
	"CTFOptionsDialog"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFOptionsDialog"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"f0"
		"tall"				"f0"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		//"border"				"TFFatLineBorder"
	}

	"Background"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"Background"
		"xpos"				"-10"
		"ypos"				"60"
		"zpos"				"-1"
		"wide"				"870"
		"tall"				"350"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"border"			"AdvSquareButtonDefault"
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

	"OptionLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"OptionLabel"
		"xpos"				"20"
		"ypos"				"35"
		"zpos"				"5"
		"wide"				"600"
		"tall"				"25"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"#GameUI_GameMenu_Options"
		"textAlignment"		"west"
		"font"				"HudFontBiggerBold"
		"fgcolor"			"AdvTextDefault"
	}	

	"OK"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"OK"
		"xpos"			"352"
		"ypos"			"420"
		"zpos"			"2"
		"wide"			"124"
		"tall"			"25"
		"visible"		"1"
		"enabled"		"1"
		"command"		"Ok"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_OK"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"border_default"	"AdvLeftButtonDefault"
			"border_armed"		"AdvLeftButtonArmed"
			"border_depressed"	"AdvLeftButtonDepressed"
			"xshift" 			"0"
			"yshift"			"-3"
		}
	}
	
	"Cancel"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"Cancel"
		"xpos"			"476"
		"ypos"			"420"
		"zpos"			"2"
		"wide"			"124"
		"tall"			"25"
		"visible"		"1"
		"enabled"		"1"
		"command"		"vguicancel"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_Cancel"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"border_default"	"AdvSquareButtonDefault"
			"border_armed"		"AdvSquareButtonArmed"
			"border_depressed"	"AdvSquareButtonDepressed"
			"xshift" 			"0"
			"yshift"			"-3"
		}
	}	
	
	"Apply"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"Apply"
		"xpos"			"600"
		"ypos"			"420"
		"zpos"			"2"
		"wide"			"124"
		"tall"			"25"
		"visible"		"1"
		"enabled"		"1"
		"command"		"Apply"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_Apply"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"border_default"	"AdvRightButtonDefault"
			"border_armed"		"AdvRightButtonArmed"
			"border_depressed"	"AdvRightButtonDepressed"
			"xshift" 			"0"
			"yshift"			"-3"
		}
	}

	"Defaults"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"Defaults"
		"xpos"				"180"
		"ypos"				"420"
		"wide"				"124"
		"tall"				"25"
		"visible"			"1"
		"enabled"			"1"
		"Command"			"DefaultsOK"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_UseDefaults"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"
			"xshift" 			"0"
			"yshift"			"-3"
		}
	}

	"CancelX"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"CancelX"
		"xpos"			"825"
		"ypos"			"0"
		"zpos"			"20"
		"wide"			"30"
		"tall"			"25"
		"visible"		"1"
		"enabled"		"1"
		"command"		"vguicancel"
		"bordervisible"		"0"
		
		"SubButton"
		{
			"labelText" 		"X"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDepressed"
			"armedFgColor_override"			"AdvTextArmed"
			"depressedFgColor_override"		"MainMenuTextDefault"
		}
	}

	"optionsbutton"
	{
		"ControlName"		"CAdvTabs"
		"fieldName"			"optionsbutton"
		"xpos"				"180"
		"ypos"				"35"
		"zpos"				"6"
		"wide"				"500"
		"tall"				"25"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"		
		
		"AdvButton"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"AdvButton"
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"			"1"		
			"command"				"newoptionsadv"
			
			"SubButton"
			{
				"labelText"			"#GameUI_Multiplayer"
				"xshift" 			"0"
				"yshift"			"-3"
				"stayselectedonclick"	"1"
				"textAlignment"		"center"	
				"font"				"HudFontSmallBold"
				"border_default"		"AdvLeftButtonDefault"
				"border_armed"			"AdvLeftButtonArmed"
				"border_depressed"		"AdvLeftButtonDepressed"
				"selectedFgColor_override"		"255 255 180 255"
			}
		}
		
		
		"MouseButton"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"MouseButton"
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"			"1"	
			"command"				"newoptionsmouse"
			
			"SubButton"
			{
				"labelText"			"#GameUI_Mouse"
				"xshift" 			"0"
				"yshift"			"-3"
				"stayselectedonclick"	"1"
				"textAlignment"		"center"	
				"font"				"HudFontSmallBold"
				"border_default"		"AdvSquareButtonDefault"
				"border_armed"			"AdvSquareButtonArmed"
				"border_depressed"		"AdvSquareButtonDepressed"	
				"selectedFgColor_override"		"255 255 180 255"
			}
		}
		
		"KeyboardButton"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"KeyboardButton"
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"			"1"	
			"command"				"newoptionskeyboard"
			
			"SubButton"
			{
				"labelText"			"#GameUI_Keyboard"
				"xshift" 			"0"
				"yshift"			"-3"	
				"stayselectedonclick"	"1"
				"textAlignment"		"center"
				"font"				"HudFontSmallBold"
				"border_default"		"AdvSquareButtonDefault"
				"border_armed"			"AdvSquareButtonArmed"
				"border_depressed"		"AdvSquareButtonDepressed"
				"selectedFgColor_override"		"255 255 180 255"	
			}
		}
		
		"AudioButton"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"AudioButton"
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"			"1"	
			"command"				"newoptionsaudio"
			
			"SubButton"
			{
				"labelText"			"#GameUI_Audio"
				"xshift" 			"0"
				"yshift"			"-3"
				"stayselectedonclick"	"1"
				"textAlignment"		"center"	
				"font"				"HudFontSmallBold"
				"border_default"		"AdvSquareButtonDefault"
				"border_armed"			"AdvSquareButtonArmed"
				"border_depressed"		"AdvSquareButtonDepressed"	
				"selectedFgColor_override"		"255 255 180 255"
			}
		}
		
		"VideoButton"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"VideoButton"
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"			"1"	
			"command"				"newoptionsvideo"
			
			"SubButton"
			{
				"labelText"			"#GameUI_Video"
				"xshift" 			"0"
				"yshift"			"-3"
				"stayselectedonclick"	"1"
				"textAlignment"		"center"	
				"font"				"HudFontSmallBold"
				"border_default"		"AdvRightButtonDefault"
				"border_armed"			"AdvRightButtonArmed"
				"border_depressed"		"AdvRightButtonDepressed"	
				"selectedFgColor_override"		"255 255 180 255"
			}
		}
	}

}
