"Resource/UI/main_menu/MainMenuPanel.res"
{
	"CTFMainMenuPanel"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFMainMenuPanel"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"f0"
		"tall"				"f0"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
	}	
	
	"FakeBGImage"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"FakeBGImage"
		"xpos"			"0"
		"ypos"			"0"	
		"zpos"			"200"		
		"wide"			"f0"
		"tall"			"f0"
		"visible"		"0"
		"enabled"		"1"
		"image"			"../console/background03_widescreen"
		"alpha"			"255"
		"scaleImage"	"1"	
	}	
	
	"Logo"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Logo"
		"xpos"			"30"
		"ypos"			"30"
		"zpos"			"1"
		"wide"			"265"
		"tall"			"70"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../logo/tf2_logo"
		//"image"			"../logo/tf2_logo_v2"
		//"image"			"../logo/tf2_logo_v3"
		"alpha"			"255"
		"scaleImage"	"1"	
	}
	
	"LogoCircle"
	{
		"ControlName"	"CTFRotatingImagePanel"
		"fieldName"		"LogoCircle"
		"xpos"			"164"
		"ypos"			"64"
		"zpos"			"5"
		"wide"			"30"
		"tall"			"30"
		"image"			"../logo/tf2_logo_icon"
		"visible"		"0"
		"enabled"		"0"
	}
	
	"MainMenuBGPanel"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"MainMenuBGPanel"
		"xpos"			"30"
		"ypos"			"160"
		"zpos"			"-1"
		"wide"			"265"
		"tall"			"60"
		"visible"		"1"
		"pinCorner"		"0"
		"autoResize"	"0"
		"PaintBackgroundType"	"2"
		"border"		"MainMenuBGBorder"
		//"bgcolor_override"	"100 255 150 55"
		"mouseinputenabled"	"0"
	}
	"PlayLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"PlayLabel"
		"font"			"HudFontSmallBold"
		"labelText"		"#MMenu_Play"
		"textAlignment"	"west"
		"xpos"			"40"
		"ypos"			"160"
		"wide"			"140"
		"tall"			"30"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"fgcolor_override"	"235 227 203 255"
	}

	"BlogPanel"
	{
		"ControlName"		"CTFBlogPanel"
		"fieldName"			"BlogPanel"
		"xpos"				"r420"
		"ypos"				"65"
		"zpos"				"3"
		"wide"				"300"
		"tall"				"350"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuAdvButtonDepressed"	
	}

	"BackgroundFooter"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"BackgroundFooter"
		"xpos"			"0"
		"ypos"			"420"
		"zpos"			"-50"
		"wide"			"f0"
		"tall"			"60"
		"visible"		"1"
		"enabled"		"1"
		"image"			"loadout_bottom_gradient"
		"tileImage"		"1"
	}				
	"FooterLine"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"FooterLine"
		"xpos"			"0"
		"ypos"			"420"
		"zpos"			"-5-"
		"wide"			"f0"
		"tall"			"10"
		"visible"		"1"
		"enabled"		"1"
		"image"			"loadout_solid_line"
		"scaleImage"	"1"
	}

	"AvatarBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"AvatarBG"
		"xpos"				"35"
		"ypos"				"105"
		"zpos"				"5"
		"wide"				"50"
		"tall"				"50"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuBGBorderAlpha"
		"font"				"MenuMainTitle"
		"bgcolor_override"	"17 67 44 255"
	}
	
	"AvatarImage"
	{
		"ControlName"	"CAvatarImagePanel"
		"fieldName"		"AvatarImage"
		"xpos"			"40"
		"ypos"			"110"
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
		"ypos"				"105"
		"zpos"				"4"
		"wide"				"255"
		"tall"				"50"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuBGBorder"
		"font"				"MenuMainTitle"
	}

	"MenuBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"MenuBG"
		"xpos"				"25"
		"ypos"				"-10"
		"zpos"				"-100"
		"wide"				"275"
		"tall"				"f0"
		"visible"			"1"
		"enabled"			"1"
		"border"			"AdvRoundedButtonDefault"
		"font"				"MenuMainTitle"
	}
	
	"WelcomeLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"WelcomeLabel"
		"xpos"				"90"
		"ypos"				"108"
		"zpos"				"6"
		"wide"				"190"
		"tall"				"20"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"alpha"				"255"
		"labelText"			"#WelcomeBack2"
		"textAlignment"		"west"
		"font"				"HudFontMediumSmallBold"
		"fgcolor"			"AdvTextDefault"
	}
	
	"NicknameLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"NicknameLabel"
		"xpos"				"90"
		"ypos"				"125"
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
	
	"ServerBrowserButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"ServerBrowserButton"
		"xpos"					"35"
		"ypos"					"190"
		"zpos"					"5"
		"wide"					"254"
		"tall"					"25"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"		
		"command"				"gamemenucommand OpenServerBrowser"		
		
		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_FindServers"
			"xshift" 			"0"
			"yshift" 			"-2"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			//"defaultFgColor_override"		"MainMenuTextDefault"
			//"armedFgColor_override"			"MainMenuTextArmed"
			//"depressedFgColor_override"		"MainMenuTextDepressed"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"
		}
		
		"SubImage"
		{
			"image" 			"glyph_server_browser"
			"imagewidth"		"16"	
		}
	}

	"SinglePlayerButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"SinglePlayerButton"
		"xpos"					"165"
		"ypos"					"190"
		"zpos"					"11"
		"wide"					"124"
		"tall"					"25"
		"visible"				"0"
		"enabled"				"0"
		"bordervisible"			"1"	
		"command"				""

		"SubButton"
		{
			"labelText" 		"???"
			"xshift" 			"0"
			"yshift" 			"-2"
			"textAlignment"		"west"	
			"font"				"HudFontSmallBold"
			//"defaultFgColor_override"		"MainMenuTextDefault"
			//"armedFgColor_override"			"MainMenuTextArmed"
			//"depressedFgColor_override"		"MainMenuTextDepressed"	
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextDefault"
			"depressedFgColor_override"		"MainMenuTextDefault"	
			"border_default"	"MainMenuAdvButtonDepressed"
			"border_armed"		"MainMenuAdvButtonDepressed"
			"border_depressed"	"MainMenuAdvButtonDepressed"	
		}

		"SubImage"
		{
			"image" 			"glyph_practice"
			"imagewidth"		"16"
		}		
	}

	"MultiplayerGameButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"MultiplayerGameButton"
		"xpos"					"265"
		"ypos"					"190"
		"zpos"					"12"
		"wide"					"24"
		"tall"					"24"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"
		//"command"			"newcreategame"
		"command"			"gamemenucommand OpenCreateMultiplayerGameDialog"
		
		"SubButton"
		{
			"labelText" 		""
			"tooltip" 			"#MMenu_PlayList_CreateServer_Button"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextArmed"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDefault"	
			"border_default"	"AdvRoundedButtonArmed"
			"border_armed"		"AdvRoundedButtonDepressed"
			"border_depressed"	"AdvRoundedButtonDefault"
		}
		
		"SubImage"
		{
			"image" 			"glyph_create"
			"imagewidth"		"16"	
		}
	}

	"CustomizeBGPanel"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"StoreBGPanel"
		"xpos"			"30"
		"ypos"			"230"
		"zpos"			"-1"
		"wide"			"265"
		"tall"			"60"
		"visible"		"1"
		"PaintBackgroundType"	"2"
		"border"		"MainMenuBGBorder"
		
		"CustomizeLabel"
		{
			"ControlName"	"CExLabel"
			"fieldName"		"CustomizeLabel"
			"font"			"HudFontSmallBold"
			"labelText"		"#MMenu_Customize"
			"textAlignment"	"west"
			"xpos"			"10"
			"ypos"			"0"
			"wide"			"140"
			"tall"			"30"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor_override"	"235 227 203 255"
		}
	}

	"LoadoutButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"LoadoutButton"
		"xpos"					"35"
		"ypos"					"260"
		"zpos"					"11"
		"wide"					"124"
		"tall"					"25"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"	
		"command"				"newloadout"
		
		"SubButton"
		{
			"labelText" 		"Loadout"
			"xshift" 			"0"
			"yshift" 			"-2"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			//"defaultFgColor_override"		"MainMenuTextDefault"
			//"armedFgColor_override"			"MainMenuTextArmed"
			//"depressedFgColor_override"		"MainMenuTextDepressed"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"
		}

		"SubImage"
		{
			"image" 			"glyph_items"
			"imagewidth"		"16"	
		}
	}
	
	"OptionsDialogButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"OptionsDialogButton"
		"xpos"					"165"
		"ypos"					"260"
		"zpos"					"11"
		"wide"					"124"
		"tall"					"25"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"	
		"command"				"newoptionsdialog"

		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Options"
			"xshift" 			"0"
			"yshift" 			"-2"
			"textAlignment"		"west"	
			"font"				"HudFontSmallBold"
			//"defaultFgColor_override"		"MainMenuTextDefault"
			//"armedFgColor_override"			"MainMenuTextArmed"
			//"depressedFgColor_override"		"MainMenuTextDepressed"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"
		}

		"SubImage"
		{
			"image" 			"glyph_options"
			"imagewidth"		"16"
		}		
	}

	"AchievementStatsBGPanel"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"AchievementStatsBGPanel"
		"xpos"			"30"
		"ypos"			"300"
		"zpos"			"-1"
		"wide"			"265"
		"tall"			"60"
		"visible"		"1"
		"PaintBackgroundType"	"2"
		"border"		"MainMenuBGBorder"
		
		"AchievementStatsLabel"
		{
			"ControlName"	"CExLabel"
			"fieldName"		"AchievementStatsLabel"
			"font"			"HudFontSmallBold"
			"labelText"		"#MMenu_Other"
			"textAlignment"	"west"
			"xpos"			"10"
			"ypos"			"0"
			"wide"			"150"
			"tall"			"30"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor_override"	"235 227 203 255"
		}
	}

	"AchievementsButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"AchievementsButton"
		"xpos"					"35"
		"ypos"					"310"
		"zpos"					"11"
		"wide"					"124"
		"tall"					"25"
		"visible"				"0"
		"enabled"				"0"
		"bordervisible"			"1"	
		"command"				"gamemenucommand OpenAchievementsDialog"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Achievements"
			"xshift" 			"0"
			"yshift" 			"-1"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}

		"SubImage"
		{
			"image" 			"glyph_achievements"
			"imagewidth"		"16"	
		}
	}

	"StatsButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"StatsButton"
		"xpos"					"165"
		"ypos"					"310"
		"zpos"					"11"
		"wide"					"124"
		"tall"					"25"
		"visible"				"0"
		"enabled"				"0"
		"bordervisible"			"1"	
		"command"				"newstats"

		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_PlayerStats"
			"xshift" 			"0"
			"yshift" 			"-1"
			"textAlignment"		"west"	
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}

		"SubImage"
		{
			"image" 			"main_menu/glyph_stats"
			"imagewidth"		"16"
		}
	}

	"SmallButtonBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"SmallButtonBG"
		"xpos"				"35"
		"ypos"				"310"
		"zpos"				"4"
		"wide"				"200"
		"tall"				"40"
		"visible"			"0"
		"enabled"			"0"
		"border"			"AdvRoundedButtonDefault"
		//"font"			"MenuMainTitle"

		"InnerShadow"
		{
			"ControlName"	"EditablePanel"
			"fieldname"		"InnerShadow"
			"xpos"			"10"
			"ypos"			"4"
			"zpos"			"0"
			"wide"			"120"
			"tall"			"30"
			"visible"		"0"	
			"PaintBackgroundType"	"0"
			"proportionaltoparent"	"1"
			"mouseinputenabled"	"0"

			"paintborder"	"1"
			"border"		"StoreInnerShadowBorder"
		}

		"BelowDarken"
		{
			"ControlName"	"EditablePanel"
			"fieldname"		"BelowDarken"
			"xpos"			"10"
			"ypos"			"4"
			"zpos"			"0"
			"wide"			"120"
			"tall"			"30"
			"visible"		"0"	
			"PaintBackgroundType"	"0"
			"proportionaltoparent"	"1"
			"mouseinputenabled"	"0"

			"bgcolor_override"	"0 0 0 100"
		}
	}

	"SmallAchievementsButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"SmallAchievementsButton"
		"xpos"				"35"
		"ypos"				"325"
		"zpos"				"5"
		"wide"				"30"
		"tall"				"30"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"1"
		"labelText" 		""
		"Command"		"gamemenucommand OpenAchievementsDialog"

		"SubButton"
		{
			"labelText" 		""
			"bordervisible"		"0"
			"xshift" 			"0"
			"yshift" 			"-1"
			"tooltip" 			"#MMenu_Tooltip_Achievements"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"border_default"	"AdvLeftButtonDefault"
			"border_armed"		"AdvLeftButtonArmed"
			"border_depressed"	"AdvLeftButtonDepressed"
		}

		"SubImage"
		{
			"image" 			"glyph_achievements"
			"imagewidth"		"24"
		}	
	}

	"CreditsButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"CreditsButton"
		"xpos"			"65"
		"ypos"			"325"
		"zpos"			"5"
		"wide"			"30"
		"tall"			"30"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"
		"labelText"		""
		"font"			"HudFontSmallBold"
		"Command"		"newcredits"
		"bordervisible"		"1"

		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"

		"image_drawcolor"	"235 226 202 255"

		"SubButton"
		{
			"labelText" 		""
			"xshift" 			"0"
			"yshift" 			"-1"
			"bordervisible"		"0"
			"tooltip" 			"Credits"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"border_default"	"AdvSquareButtonDefault"
			"border_armed"		"AdvSquareButtonArmed"
			"border_depressed"	"AdvSquareButtonDepressed"
		}

		"SubImage"
		{
			"scaleImage"	"1"
			"image" 			"glyph_multiplayer"
			"imagewidth"		"22"
		}				
	}

	"SmallStatsButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"SmallStatsButton"
		"xpos"			"95"
		"ypos"			"325"
		"zpos"			"5"	
		"wide"			"30"
		"tall"			"30"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"
		"labelText"		""
		"font"			"HudFontSmallBold"
		"Command"		"newstats"
		"bordervisible"		"1"

		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"

		"image_drawcolor"	"235 226 202 255"

		"SubButton"
		{
			"labelText" 		""
			"xshift" 			"0"
			"yshift" 			"-1"
			"bordervisible"		"0"
			"tooltip" 			"#GameUI_GameMenu_PlayerStats"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"border_default"	"AdvSquareButtonDefault"
			"border_armed"		"AdvSquareButtonArmed"
			"border_depressed"	"AdvSquareButtonDepressed"
		}

		"SubImage"
		{
			"scaleImage"	"1"
			"image" 			"main_menu/glyph_stats"
			"imagewidth"		"24"
		}				
	}

	"OptionsOldButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"OptionsOldButton"		
		"xpos"					"125"
		"ypos"					"325"
		"zpos"					"5"
		"wide"					"30"
		"tall"					"30"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"labelText"				""
		"font"					"HudFontSmallBold"
		"command"				"gamemenucommand openoptionsdialog"

		"SubButton"
		{
			"labelText" 		""
			"xshift" 			"0"
			"yshift" 			"-1"
			"tooltip" 			"#MMenu_Tooltip_OldOptions"
			"bordervisible"		"0"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"border_default"	"AdvRightButtonDefault"
			"border_armed"		"AdvRightButtonArmed"
			"border_depressed"	"AdvRightButtonDepressed"
		}

		"SubImage"
		{
			"image" 			"glyph_steamworkshop"
			"imagewidth"		"16"
		}
	}

	"QuitButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"QuitButton"
		"xpos"					"35"
		"ypos"					"430"
		"zpos"					"1"
		"wide"					"124"
		"tall"					"25"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"	
		"command"				"newquit"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Quit"
			"bordervisible"		"0"
			"xshift" 			"0"
			"yshift" 			"-2"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			//"defaultFgColor_override"		"MainMenuTextDefault"
			//"armedFgColor_override"			"MainMenuTextArmed"
			//"depressedFgColor_override"		"MainMenuTextDepressed"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonDepressed"
			"border_depressed"	"AdvRoundedButtonArmed"
		}
		
		"SubImage"
		{
			"image" 			"glyph_quit"
			"imagewidth"		"14"
		}
	}
	
	"MusicToggleCheck"
	{
		"ControlName"		"CTFAdvCheckButton"
		"fieldName"			"MusicToggleCheck"
		"xpos"				"c240"		//+19
		"ypos"				"30"
		"zpos"				"6"
		"wide"				"32"
		"tall"				"32"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"1"
		"labelText" 		""
		"command"			"lfe_mainmenu_music"	
		"valuetrue"			"0"
		"valuefalse"		"1"		
		
		"SubButton"
		{
			"labelText" 		""
			"xshift" 			"0"
			"yshift" 			"-1"
			"bordervisible"		"0"
			"tooltip" 			"#MMenu_Tooltip_ToggleRandomMusic"
			"textAlignment"		"center"
			"font"				"MenuSmallFont"
			"border_default"	"AdvLeftButtonDefault"
			"border_armed"		"AdvLeftButtonArmed"
			"border_depressed"	"AdvLeftButtonDepressed"
		}
		
		"SubImage"
		{
			"image" 			"main_menu/glyph_speaker"
			"imagecheck" 		"main_menu/glyph_disabled"	
			"imagewidth"		"24"
		}
	}
	
	"RandomMusicButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"RandomMusicButton"
		"xpos"				"c271"
		"ypos"				"30"
		"zpos"				"5"
		"wide"				"32"
		"tall"				"32"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"1"
		"labelText" 		""
		"command"			"randommusic"			
		
		"SubButton"
		{
			"labelText" 		""
			"xshift" 			"0"
			"yshift" 			"-1"
			"bordervisible"		"0"
			"tooltip" 			"#MMenu_Tooltip_RandomMusic"
			"textAlignment"		"center"
			"font"				"MenuSmallFont"
			"border_default"	"AdvRightButtonDefault"
			"border_armed"		"AdvRightButtonArmed"
			"border_depressed"	"AdvRightButtonDepressed"
		}
		
		"SubImage"
		{
			"image" 			"main_menu/glyph_random"
			"imagewidth"		"24"
		}
	}

	"BlogToggleCheck"
	{
		"ControlName"		"CTFAdvCheckButton"
		"fieldName"			"BlogToggleCheck"
		"xpos"				"c200"
		"ypos"				"30"
		"zpos"				"10"
		"wide"				"32"
		"tall"				"32"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"1"
		"labelText" 		""
		"command"			"lfe_mainmenu_showblog"	
		"valuetrue"			"0"
		"valuefalse"		"1"		
		
		"SubButton"
		{
			"labelText" 		""
			"xshift" 			"0"
			"yshift" 			"-1"
			"bordervisible"		"0"
			"tooltip" 			"#MMenu_Tooltip_ToggleBlog"
			"textAlignment"		"center"
			"font"				"MenuSmallFont"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"
		}
		
		"SubImage"
		{
			"image" 			"glyph_tv"
			"imagecheck" 		"main_menu/glyph_disabled"	
			"imagewidth"		"24"
		}
	}

	"NotificationButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"NotificationButton"
		"xpos"				"263"
		"ypos"				"340"
		"zpos"				"6"
		"wide"				"20"
		"tall"				"20"
		"visible"			"0"
		"enabled"			"0"
		"bordervisible"		"1"	
		"command"			"shownotification"	
		
		"SubButton"
		{
			"labelText" 		""
			"xshift" 			"0"
			"yshift" 			"-1"
			"bordervisible"		"0"
			"tooltip" 			"Show notification"
			"textAlignment"		"center"
			"font"				"MenuSmallFont"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"
		}
		
		"SubImage"
		{
			"image" 			"glyph_alert"
			"imagewidth"		"16"
		}
	}		
	
	"VersionLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"VersionLabel"
		"xpos"				"r525"
		"ypos"				"0"
		"zpos"				"5"
		"wide"				"520"
		"tall"				"40"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"0"
		"enabled"			"0"
		"labelText"		"VersionLabel"
		"textAlignment"		"east"
		"fgcolor"			"HudOffWhite"
		"font"				"MenuSmallFont"
	}

	"VersionLabelVerText"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"VersionLabelVerText"
		"xpos"				"r540"
		"ypos"				"425"
		"zpos"				"5"
		"wide"				"520"
		"tall"				"40"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"		"Version: 4.0 Beta"
		"textAlignment"		"east"
		"fgcolor"			"HudOffWhite"
		"font"				"MenuSmallFont"
	}
}