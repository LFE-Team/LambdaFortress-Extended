"resource/ui/mainmenuoverride.res"
{
	"CTFMainMenu"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFMainMenu"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"f0"
		"tall"				"f0"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
	}
	"Background"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"Background"
		"xpos"			"0"
		"ypos"			"0"
		"zpos"			"1"
		"wide"			"f0"
		"tall"			"480"
		"visible"		"1"
		"enabled"		"1"
		"image"			""
		"scaleImage"		"1"
	}

	"Logo"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Logo"
		"xpos"			"c-600"
		"ypos"			"32"
		"zpos"			"1"
		"wide"			"512"
		"tall"			"128"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../logo/tf2_logo"
		"scaleImage"	"1"
		"mouseinputenabled"	"0"
	}

	//Expanded, "state"
	//ButtonToggled, "state"
	//"ExpandGamesButton"
	//{
	//	"ControlName"	"CTFExpandButton"
	//	"fieldName"		"ExpandGamesButton"
	//	"xpos"			"40"
	//	"ypos"			"r0"
	//	"zpos"			"6"
	//	"wide"			"125"
	//	"tall"			"25"
	//	"autoResize"	"0"
	//	"pinCorner"		"3"
	//	"visible"		"1"
	//	"enabled"		"1"
	//	"tabPosition"	"0"
	//	"labelText"		"Find a Game"
	//	"textAlignment"	"west"
	//	"textinsetx"	"25"
	//	"dulltext"		"0"
	//	"brighttext"	"0"

	//	"font"			"HudFontSmallBold"
	//	"fgcolor"		"Black"

	//	"sound_depressed"	"UI/buttonclick.wav"
	//	"sound_released"	"UI/buttonclickrelease.wav"

	//	"Expanded"		"1"
	//}

	"AvatarBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"AvatarBG"
		"xpos"				"35"
		"ypos"				"r400"
		"zpos"				"5"
		"wide"				"50"
		"tall"				"50"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuBGBorderAlpha"
		"font"				"MenuMainTitle"
		"bgcolor_override"	"117 107 94 255"
	}
	
	"AvatarImage"
	{
		"ControlName"	"CAvatarImagePanel"
		"fieldName"		"AvatarImage"
		"xpos"			"40"
		"ypos"				"r400"
		"zpos"			"6"
		"wide"			"40"
		"tall"			"40"
		"visible"		"1"
		"enabled"		"1"
		"image"			""
		"scaleImage"		"1"	
		"color_outline"		"52 48 45 255"
	}	
	
	"WelcomeBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"WelcomeBG"
		"xpos"				"40"
		"ypos"				"r395"
		"zpos"				"4"
		"wide"				"245"
		"tall"				"50"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuBGBorderAlpha"
		"font"				"MenuMainTitle"
		"bgcolor_override"	"36 33 32 255"
	}
	
	"WelcomeLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"WelcomeLabel"
		"xpos"				"90"
		"ypos"				"r400"
		"zpos"				"6"
		"wide"				"190"
		"tall"				"20"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"alpha"				"255"
		"labelText"			"Welcome back,"
		"textAlignment"		"west"
		"font"				"HudFontMediumSmallBold"
		"fgcolor"			"AdvTextDefault"
	}
	
	"WelcomeLabel1"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"WelcomeLabel1"
		"xpos"				"90"
		"ypos"				"r400"
		"zpos"				"5"
		"wide"				"190"
		"tall"				"20"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"alpha"				"0"
		"labelText"			"Welcome back,"
		"textAlignment"		"west"
		"font"				"HudFontMediumSmallBold_Shadow"
		"fgcolor"			"AdvTextDefault"
	}	
	
	"NicknameLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"NicknameLabel"
		"xpos"				"90"
		"ypos"				"r420"
		"zpos"				"5"
		"wide"				"190"
		"tall"				"20"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"%nickname%"
		"textAlignment"		"west"
		"font"				"HudFontMediumSmallBold"
		"fgcolor"			"AdvTextDefault"
	}

	"MainMenuBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"MainMenuBG"
		"xpos"				"30"
		"ypos"				"r380"
		"zpos"				"3"
		"wide"				"300"
		"tall"				"300"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuBGBorderAlpha"
		"font"				"MenuMainTitle"
	}

	"PlayListContainer"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"PlayListContainer"
		"xpos"				"30"
		"ypos"				"r380"
		"zpos"				"3"
		"wide"				"270"
		"tall"				"0"
		"visible"		"0"

		"PlaylistBGPanel"
		{
			"ControlName"	"EditablePanel"
			"fieldName"		"PlaylistBGPanel"
			"xpos"				"30"
			"ypos"				"r380"
			"zpos"				"3"
			"wide"				"270"
			"tall"				"300"
			"visible"			"1"
			"PaintBackgroundType"	"2"
			"border"		"MainMenuBGBorder"
			"proportionaltoparent"	"1"

			"pinCorner"		"2"
			"autoResize"	"1"
			
			"FindServerButton"
			{
				"ControlName"	"CTFButton"
				"fieldName"		"FindServerButton"
				"xpos"			"40"
				"ypos"			"r350"
				"zpos"			"6"
				"wide"			"250"
				"tall"			"40"
				"autoResize"	"0"
				"pinCorner"		"3"
				"visible"		"1"
				"enabled"		"1"
				"tabPosition"	"0"
				"labelText"		"#GameUI_GameMenu_FindServers"
				"textAlignment"	"west"
				"textinsetx"	"25"
				"dulltext"		"0"
				"brighttext"	"0"
				"command"		"gamemenucommand OpenServerBrowser"
				"font"			"HudFontSmallBold"
				"fgcolor"		"Black"

				"sound_depressed"	"UI/buttonclick.wav"
				"sound_released"	"UI/buttonclickrelease.wav"
			}
		}
	}

	"MuteButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"MuteButton"
		"xpos"				"250"
		"ypos"				"198"
		"zpos"				"6"
		"wide"				"24"
		"tall"				"24"
		"visible"			"1"
		"enabled"			"1"
		"command"			"gamemenucommand openplayerlistdialog"
		
		"SubButton"
		{
			"labelText" 		""
			"tooltip" 			"Mute Players"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
			"border_default"				"MainMenuMiniButtonDefault"
			"border_depressed"				"MainMenuMiniButtonArmed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_muted"
			"imagewidth"		"16"
			"image_drawcolor"	"MainMenuTextDefault"
			"image_armedcolor"	"MainMenuTextArmed"
			"image_armeddepressed"	"MainMenuTextDepressed"
		}
	}

	"ResumeButtonHalf"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"ResumeButtonHalf"
		"xpos"			"180"
		"ypos"			"r350"
		"zpos"			"6"
		"wide"			"140"
		"tall"			"40"
		"visible"		"0"
		"enabled"		"1"

		"command"		"gamemenucommand ResumeGame"

		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"

		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_ResumeGame"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_resume"
			"imagewidth"		"18"
			"image_drawcolor"	"MainMenuTextDefault"
			"image_armedcolor"	"MainMenuTextArmed"
			"image_armeddepressed"	"MainMenuTextDepressed"
		}
	}

	"FindAGameButtonHalf"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"FindAGameButtonHalf"
		"xpos"			"40"
		"ypos"			"r350"
		"zpos"			"6"
		"wide"			"140"
		"tall"			"40"
		"visible"		"0"
		"enabled"		"1"
		"command"		"gamemenucommand OpenServerBrowser"

		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"

		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_FindServers"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_server_browser"
			"imagewidth"		"18"
			"image_drawcolor"	"MainMenuTextDefault"
			"image_armedcolor"	"MainMenuTextArmed"
			"image_armeddepressed"	"MainMenuTextDepressed"
		}
	}

	"FindAGameButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"FindAGameButton"
		"xpos"			"40"
		"ypos"			"r350"
		"zpos"			"6"
		"wide"			"280"
		"tall"			"40"
		"visible"		"1"
		"enabled"		"1"
		"command"		"gamemenucommand OpenServerBrowser"

		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"

		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_FindServers"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_server_browser"
			"imagewidth"		"18"
			"image_drawcolor"	"MainMenuTextDefault"
			"image_armedcolor"	"MainMenuTextArmed"
			"image_armeddepressed"	"MainMenuTextDepressed"
		}
	}

	"MultiplayerGameButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"MultiplayerGameButton"
		"xpos"					"250"
		"ypos"					"r350"
		"zpos"					"7"
		"wide"					"30"
		"tall"					"30"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"
		"command"				"gamemenucommand OpenCreateMultiplayerGameDialog"
		
		"SubButton"
		{
			"labelText" 		""
			"tooltip" 			"Create Server"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
			"border_default"				"MainMenuMiniButtonDefault"
			"border_depressed"				"MainMenuMiniButtonArmed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_create"
			"imagewidth"		"16"
			"image_drawcolor"	"MainMenuTextDefault"
			"image_armedcolor"	"MainMenuTextArmed"
			"image_armeddepressed"	"MainMenuTextDepressed"
		}
	}

	"AchievementsButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"AchievementsButton"
		"xpos"			"40"
		"ypos"			"r300"
		"zpos"			"6"
		"wide"			"280"
		"tall"			"40"
		"visible"		"1"
		"enabled"		"1"
		"command"		"gamemenucommand OpenAchievementsDialog"
		"fgcolor"		"Black"

		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"

		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Achievements"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_achievements"
			"imagewidth"		"18"
			"image_drawcolor"	"MainMenuTextDefault"
			"image_armedcolor"	"MainMenuTextArmed"
			"image_armeddepressed"	"MainMenuTextDepressed"
		}
	}

	"PlayerStatsButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"PlayerStatsButton"
		"xpos"			"40"
		"ypos"			"r250"
		"zpos"			"6"
		"wide"			"280"
		"tall"			"40"
		"visible"		"1"
		"enabled"		"1"
		"command"		"showstatsdlg"
		"fgcolor"		"Black"

		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"

		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_PlayerStats"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_practice"
			"imagewidth"		"18"
			"image_drawcolor"	"MainMenuTextDefault"
			"image_armedcolor"	"MainMenuTextArmed"
			"image_armeddepressed"	"MainMenuTextDepressed"
		}
	}

	"OptionsButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"OptionsButton"
		"xpos"			"40"
		"ypos"			"r200"
		"zpos"			"6"
		"wide"			"280"
		"tall"			"40"
		"visible"		"1"
		"enabled"		"1"
		"command"		"gamemenucommand OpenOptionsDialog"
		"font"			"HudFontSmallBold"

		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"

		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Options"
			"xshift" 			"10"
			"yshift" 			"0"	
			"textAlignment"		"west"	
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_options"
			"imagewidth"		"18"
			"image_drawcolor"	"MainMenuTextDefault"
			"image_armedcolor"	"MainMenuTextArmed"
			"image_armeddepressed"	"MainMenuTextDepressed"
		}		
	}

	"DisconnectButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"DisconnectButton"
		"xpos"			"40"
		"ypos"			"r150"
		"zpos"			"6"
		"wide"			"280"
		"tall"			"40"
		"visible"		"0"
		"enabled"		"1"
		"command"		"gamemenucommand Disconnect"
		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"

		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Disconnect"
			"bordervisible"		"0"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_quit"
			"imagewidth"		"18"
			"image_drawcolor"	"MainMenuTextDefault"
			"image_armedcolor"	"MainMenuTextArmed"
			"image_armeddepressed"	"MainMenuTextDepressed"
		}
	}

	"QuitButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"QuitButton"
		"xpos"			"40"
		"ypos"			"r150"
		"zpos"			"6"
		"wide"			"280"
		"tall"			"40"
		"visible"		"1"
		"enabled"		"1"
		"command"		"quit prompt"

		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Quit"
			"bordervisible"		"0"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_quit"
			"imagewidth"		"18"
			"image_drawcolor"	"MainMenuTextDefault"
			"image_armedcolor"	"MainMenuTextArmed"
			"image_armeddepressed"	"MainMenuTextDepressed"
		}
	}

	"BackgroundFooter"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"BackgroundFooter"
		"xpos"			"0"
		"ypos"			"640"
		"zpos"			"-50"
		"wide"			"f0"
		"tall"			"90"
		"visible"		"0"
		"enabled"		"0"
		"image"			"loadout_bottom_gradient"
		"tileImage"		"1"
	}
}