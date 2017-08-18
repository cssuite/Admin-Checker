#include <amxmodx>
#include <fvault>

static const PLUGIN[] = "Admins Checker"
static const VERSION[] = "1.0"
static const AUTHOR[] = "RevCrew"

static stock const PREFIX[] = "Admins Checker"

#define get_user_state(%1,%2) 		( %1 &   1 << ( %2 & 31 ) )
#define add_user_state(%1,%2)	 	( %1 |=  ( 1 << ( %2 & 31 ) ) )
#define remove_user_state(%1,%2)	( %1 &= ~( 1 << ( %2 & 31 ) ) )

#define LOG_FILE "addons/amxmodx/data/_FD.log"
#define STRLEN_SAY 3

static stock const DIR[] = 			"addons/amxmodx/data/AC/";
static stock const ADMIN_FILE_TIME[] = 		"admin_time";
static stock const ADMIN_FILE_COMMAND[] =	"addons/amxmodx/data/AC/admin_command_";

static stock const ADMIN_FLAG[] = 		"a";
static stock const ADMIN_FLAG_FOR_MENU[] = 	"s";

static  is_admin = 0;

const ARGV_LEN = 128;


public plugin_init()
{
        register_plugin(PLUGIN,VERSION,AUTHOR);
	
	if(!dir_exists(DIR))
		if(mkdir(DIR))
			AddLog("Success create dir [Dir::%s]",DIR)

	register_concmd("amx_acmenu", "CmdAcMenu", read_flags(ADMIN_FLAG_FOR_MENU), "<Display Admin Checker Menu>")
}
public CmdAcMenu(id)
{
	if(!has_user_access(id,read_flags(ADMIN_FLAG_FOR_MENU)))
		return client_print(id,print_console,"[%s] You Have no access to this Command",PREFIX);
	
	new menu = menu_create("\w[\rAdmin Checker\w] - \yMain Menu^n\dDisplay Admin Time on this server","HandleAcMenu")
	new f = fopen("addons/amxmodx/data/file_vault/admin_time.txt", "rt")
	
	if(!f)
		return menu_destroy(menu);
	
	new string[128], authid[26], temp[64], time[12], s_time[64], i = 0, s_i[2];
	while(!feof(f))
	{
		fgets(f, string, charsmax(string))
		
		if(!string[0])
			continue;
			
		parse(string, authid, charsmax(authid), temp, charsmax(temp))
		
		strtok(temp, temp, charsmax(temp), time, charsmax(time), '_')
		
		//replace_all(authid, charsmax(authid), "STEAM", "")
		//replace_all(authid, charsmax(authid), "VALVE", "")
		
		GetTime( floatround(float(str_to_num(time))/60,floatround_ceil), s_time, charsmax(s_time))
		
		formatex(string, charsmax(string), "\y%s \w(\y%s\w) - [\r%s\w]", temp, authid, s_time)
		num_to_str(++i, s_i, charsmax(s_i))
		
		menu_additem(menu, string, s_i)
	}
	fclose(f);
	
	menu_display(id,menu, 0)
	return PLUGIN_CONTINUE;
}
public HandleAcMenu(id, menu, item)
{
	if (item == MENU_EXIT)
	{
		menu_destroy(menu)
		return PLUGIN_HANDLED
	}
	
	static s_Data[12], s_Name[2], i_Access, i_Callback
	menu_item_getinfo(menu, item, i_Access, s_Data, charsmax(s_Data), s_Name, charsmax(s_Name), i_Callback)
	
	menu_destroy(menu)
	return PLUGIN_HANDLED
}
public client_putinserver(id)
{
	if(get_user_state(is_admin, id))
		remove_user_state(is_admin, id);
	
	if(is_user_admin(id))
	{
		add_user_state(is_admin, id);
		
		static name[32]
		get_user_name(id, name, charsmax(name))
	
		static authid[22]
		get_user_authid(id, authid, charsmax(authid))
		
		AddLog("Admin %s (%s) connected",name,authid)
	}
}
public client_disconnect(id)
{
	if(get_user_state(is_admin, id))
	{
		static _time= 0;
		
		static authid[22];
		get_user_authid(id, authid, charsmax(authid))
		
		static name[32];
		get_user_name(id, name, charsmax(name))
		replace_all(name,charsmax(name), "_","*")
	
		static time[12],parse_str[64];
		if(fvault_get_data(ADMIN_FILE_TIME, authid, parse_str, charsmax(parse_str)))
		{
			strtok(parse_str, parse_str, charsmax(parse_str), time, charsmax(time), '_')
			_time = str_to_num(time) + get_user_time(id);	
		}
		else
			_time = get_user_time(id);
		
		num_to_str(_time, time, 11)
		
		static temp[64]
		formatex(temp, charsmax(temp), "%s_%s",name,time)
	
		fvault_set_data(ADMIN_FILE_TIME, authid, temp)
	}
}

public client_command(id)
{
	if(!is_user_connected(id) || !get_user_state(is_admin, id))
		return PLUGIN_CONTINUE;
		
	static Argv[ARGV_LEN];
	read_argv(0,Argv, charsmax(Argv))
	if(equali(Argv,"say", STRLEN_SAY))
		return PLUGIN_CONTINUE;
	
	static LogDat[16],LogFile[64]
	get_time("%Y_%m_%d", LogDat, charsmax(LogDat));
	
	formatex(LogFile, charsmax(LogFile), "%s%s.log",ADMIN_FILE_COMMAND, LogDat)
	
	static string[156];
	get_time("%H:%M:%S", LogDat, charsmax(LogDat));
	
	static name[32]
	get_user_name(id, name, charsmax(name))
	
	static authid[22]
	get_user_authid(id, authid, charsmax(authid))
	
	static ip[16]
	get_user_ip(id, ip, charsmax(ip), 1)
	
	formatex(string, charsmax(string), " ^"%s^" | ^"%s^" | ^"%s^" | ^"%s^" | ^"%s^"",LogDat,name, authid, ip, Argv)
	
	write_file(LogFile, string);
	
	return PLUGIN_CONTINUE;
}
stock bool:is_user_admin(id)
{
	return get_user_flags(id) & read_flags(ADMIN_FLAG) ? true : false;
}
stock bool:has_user_access(id, FLAG)
{
	return get_user_flags(id) & FLAG ? true : false;
}
stock AddLog(const szMessage[], any:...)
{

	static szMsg[256];
	vformat(szMsg, charsmax(szMsg), szMessage, 2);
	
	log_amx("[%s] %s",PREFIX,szMsg)
	server_print("[%s] %s",PREFIX,szMsg)
	
	return;
}
stock GetTime(const bantime, length[], len)
{
	new minutes = bantime;
	new hours = 0;
	new days = 0;
	
	while( minutes >= 60 )
	{
		minutes -= 60;
		hours++;
	}
	
	while( hours >= 24 )
	{
		hours -= 24;
		days++;
	}
	
	new bool:add_before;
	if( minutes )
	{
		formatex(length, len, "%i РјРёРЅСѓС‚(С‹)", minutes);
		
		add_before = true;
	}
	if( hours )
	{
		if( add_before )
		{
			format(length, len, "%i С‡Р°СЃ(Р°,РѕРІ), %s", hours, length);
		}
		else
		{
			formatex(length, len, "%i С‡Р°СЃ(Р°,РѕРІ)", hours);
			
			add_before = true;
		}
	}
	if( days )
	{
		if( add_before )
		{
			format(length, len, "%i РґРµРЅСЊ(СЏ,РµР№), %s", days, length);
		}
		else
		{
			formatex(length, len, "%i РґРµРЅСЊ(СЏ,РµР№)", days);
			
			add_before = true;
		}
	}
	if( !add_before )
	{
		// minutes, hours, and days = 0
		// assume permanent ban
		copy(length, len, "Invalid");
	}
}
