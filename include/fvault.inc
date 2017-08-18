#if defined _file_vault_included
	#endinput
#endif

#define _file_vault_included

/**
 * FVault was created by Exolent on 8/24/08
 * This vault system uses actual files and no modules
 * It is very flexible and has many features
 * Visit this page for more information: http://forums.alliedmods.net/showthread.php?t=76453
 */

#include <amxmodx>

#define _vault_dir "addons/amxmodx/data/file_vault"

/** 
 * Retrieves a key name specified by its number
 * 
 * @param vaultname	Vault name to look in
 * @param keynum	Key number within the vault to find key name
 * @param key		String which key name will be copied to
 * @param len		Length of key name
 * @return		Returns 1 on success, 0 on failue.
 */
stock fvault_get_keyname(const vaultname[], const keynum, key[], len)
{
	new filename[128];
	_FormatVaultName(vaultname, filename, sizeof(filename) - 1);
	
	if( !file_exists(filename) )
	{
		return 0;
	}
	
	new vault = fopen(filename, "rt");
	
	new _data[64], _other[3];
	
	new line = -1;
	
	while( !feof(vault) )
	{
		fgets(vault, _data, sizeof(_data) - 1);
		
		if( ++line == keynum )
		{
			parse(_data, key, len, _other, sizeof(_other) - 1);
			
			fclose(vault);
			
			return 1;
		}
	}
	
	fclose(vault);
	
	return 0;
}

/** 
 * Retrieves a key number specified by its name
 * 
 * @param vaultname	Vault name to look in
 * @param key		Key name to search for
 * @return		Returns key number on success, -1 on failure
 */
stock fvault_get_keynum(const vaultname[], const key[])
{
	new filename[128];
	_FormatVaultName(vaultname, filename, sizeof(filename) - 1);
	
	if( !file_exists(filename) )
	{
		return -1;
	}
	
	new vault = fopen(filename, "rt");
	
	new _data[70], _key[64], _other[3];
	
	new line = -1;
	
	while( !feof(vault) )
	{
		fgets(vault, _data, sizeof(_data) - 1);
		parse(_data, _key, sizeof(_key) - 1, _other, sizeof(_other) - 1);
		
		line++;
		
		if( equal(_key, key) )
		{
			fclose(vault);
			
			return line;
		}
	}
	
	fclose(vault);
	
	return -1;
}

/** 
 * Retrieves data specified by a key
 * 
 * @param vaultname	Vault name to look in
 * @param key		Key name to look for the data
 * @param data		String which data will be copied to
 * @param len		Length of data
 * @return		Returns 1 on success, 0 on failue.
 */
stock fvault_get_data(const vaultname[], const key[], data[], len)
{
	new filename[128];
	_FormatVaultName(vaultname, filename, sizeof(filename) - 1);
	
	if( !file_exists(filename) )
	{
		return 0;
	}
	
	new vault = fopen(filename, "rt");
	
	new _data[512], _key[64];
	
	while( !feof(vault) )
	{
		fgets(vault, _data, sizeof(_data) - 1);
		parse(_data, _key, sizeof(_key) - 1, data, len);
		
		if( equal(_key, key) )
		{
			fclose(vault);
			
			return 1;
		}
	}
	
	fclose(vault);
	
	copy(data, len, "");
	
	return 0;
}

/** 
 * Sets data of a key
 * 
 * @param vaultname	Vault name to look in
 * @param key		Key name to which data will be set
 * @param data		Data to set to key
 * @return		Does not return a value.
 */
stock fvault_set_data(const vaultname[], const key[], const data[])
{
	static const temp_vault_name[] = "fvault_set_data.txt";
	
	new file = fopen(temp_vault_name, "wt");
	
	new filename[128];
	_FormatVaultName(vaultname, filename, sizeof(filename) - 1);
	
	new vault = fopen(filename, "rt");
	
	new _data[512], _key[64], _other[3];
	
	new bool:replaced = false;
	
	while( !feof(vault) )
	{
		fgets(vault, _data, sizeof(_data) - 1);
		parse(_data, _key, sizeof(_key) - 1, _other, sizeof(_other) - 1);
		
		if( equal(_key, key) && !replaced )
		{
			fprintf(file, "^"%s^" ^"%s^"^n", key, data);
			
			replaced = true;
		}
		else
		{
			fputs(file, _data);
		}
	}
	
	if( !replaced )
	{
		fprintf(file, "^"%s^" ^"%s^"^n", key, data);
	}
	
	fclose(file);
	fclose(vault);
	
	delete_file(filename);
	
	while( !rename_file(temp_vault_name, filename, 1) ) { }
	
	//delete_file(temp_vault_name);
}

/** 
 * Retrieves total keys located within the vault
 * 
 * @param vaultname	Vault name to look in
 * @return		Returns amount of keys in vault
 */
stock fvault_size(const vaultname[])
{
	new filename[128];
	_FormatVaultName(vaultname, filename, sizeof(filename) - 1);
	
	return file_exists(filename) ? file_size(filename, 1) - 1 : 0;
}

/** 
 * Retrieves a vault name specified by its number
 * 
 * @param vaultnum	Vault number to find the vault name
 * @param vaultname	String which vault name will be copied to
 * @param len		Length of vault name
 * @return		Returns 1 on success, 0 on failue.
 */
stock fvault_get_vaultname(const vaultnum, vaultname[], len)
{
	new filenum;
	
	new dir = open_dir(_vault_dir, vaultname, len);
	
	do
	{
		if( equal(vaultname, ".") || equal(vaultname, "..") )
		{
			continue;
		}
		
		if( filenum == vaultnum )
		{
			close_dir(dir);
			
			replace(vaultname, len, ".txt", "");
			
			return 1;
		}
		
		++filenum;
	}
	while( next_file(dir, vaultname, len) );
	
	close_dir(dir);
	
	copy(vaultname, len, "");
	
	return 0;
}

/** 
 * Retrieves a vault number specified by its name
 * 
 * @param vaultname	Vault name to find the number
 * @return		Returns vault number on success, -1 on failure
 */
stock fvault_get_vaultnum(const vaultname[])
{
	new filename[128], filenum;
	
	new dir = open_dir(_vault_dir, filename, sizeof(filename) - 1);
	
	do
	{
		if( equal(filename, ".") || equal(filename, "..") )
		{
			continue;
		}
		
		replace(filename, sizeof(filename) - 1, ".txt", "");
		
		if( equal(filename, vaultname) )
		{
			close_dir(dir);
			
			return filenum;
		}
		
		++filenum;
	}
	while( next_file(dir, filename, sizeof(filename) - 1) );
	
	close_dir(dir);
	
	copy(vaultname, len, "");
	
	return -1;
}

/** 
 * Retrieves total vaults ever created
 * 
 * @return		Returns amount of vaults
 */
stock fvault_total()
{
	new vaultname[128], filename[128];
	new dir = open_dir(_vault_dir, vaultname, sizeof(vaultname) - 1);
	
	new filenum;
	do
	{
		if( equal(vaultname, ".") || equal(vaultname, "..") )
		{
			continue;
		}
		
		formatex(filename, sizeof(filename) - 1, "%s/%s", _vault_dir, vaultname);
		if( file_exists(filename) )
		{
			++filenum;
		}
	}
	while( next_file(dir, vaultname, sizeof(vaultname) - 1) );
	
	close_dir(dir);
	
	return filenum;
}

/** 
 * Removes a key from a vault
 * 
 * @param vaultname	Vault name to look in
 * @param key		Key to remove
 * @return		No return
 */
stock fvault_remove_key(const vaultname[], const key[])
{
	static const temp_vault_name[] = "fvault_remove_key.txt";
	
	new filename[128];
	_FormatVaultName(vaultname, filename, sizeof(filename) - 1);
	
	if( !file_exists(filename) )
	{
		return;
	}
	
	new file = fopen(temp_vault_name, "wt");
	
	new vault = fopen(filename, "rt");
	
	new _data[512], _key[64], _other[3];
	
	while( !feof(vault) )
	{
		fgets(vault, _data, sizeof(_data) - 1);
		parse(_data, _key, sizeof(_key) - 1, _other, sizeof(_other) - 1);
		
		if( !equal(_key, key) )
		{
			fputs(file, _data);
		}
	}
	
	fclose(file);
	fclose(vault);
	
	delete_file(filename);
	
	while( !rename_file(temp_vault_name, filename, 1) ) { }
	
	//delete_file(temp_vault_name);
}

/** 
 * Clears all key entries for a vault
 * 
 * @param vaultname	Vault name to erase
 * @return		No return
 */
stock fvault_clear(const vaultname[])
{
 	new filename[128];
	_FormatVaultName(vaultname, filename, sizeof(filename) - 1);
	
	//delete_file(filename);
	
	fclose(fopen(filename, "wt"));
}
 
stock _FormatVaultName(const vaultname[], filename[], len)
{
	static const invalid_chars[][] =
	{
		"/", "\", "*", ":", "?", "^"", "<", ">", "|"
	};
	
	static tempvault[128], i;
	copy(tempvault, sizeof(tempvault) - 1, vaultname);
	
	for( i = 0; i < sizeof(invalid_chars); i++ )
	{
		replace(tempvault, sizeof(tempvault) - 1, invalid_chars[i], "");
	}
	
	if( !dir_exists(_vault_dir) )
	{
		mkdir(_vault_dir);
	}
	
	formatex(filename, len, "%s/%s.txt", _vault_dir, tempvault);
}
