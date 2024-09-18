#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "StringHash.h"

/// Initalizes the map with space for MAGIC_SIZE pointers.
StringHash::StringHash()
{
	_data = (char**)malloc(sizeof(char*) * 2 * MAGIC_SIZE);
	bool ok = _data != NULL;
	_size = 0;
	_capacity = ok ? MAGIC_SIZE : 0;
}

/// Deletes the strings at each of the pointers in the MAP, then deletes the MAP structure itself.
StringHash::~StringHash()
{
	for (int i = 0; i < _size; i++) {
		free(_data[i * 2]);
		free(_data[i * 2 + 1]);
	}
	free(_data);
}

/// Does a linear search using bid as the key and returns either NULL or the value associated with the key bid.
const char* StringHash::get(const char* bid) const
{
	for (int i = 0; i < _size; i++)
		if (!strcmp(bid, _data[i * 2]))
			return _data[i * 2 + 1];
	return NULL;
}

const char* StringHash::get(const char* bid)
{
	return const_cast<const StringHash*>(this)->get(bid);
}

/** @fn const char * StringHash::get(int index) const
  * Returns the value associated with the given key at the index provided.
  * @param index The index into the map.
  * @return NULL if given a bad index, otherwise the pointer associated with the key.
  *
  */
const char* StringHash::get(int index) const
{
	if (index >= 0 && index < _size)
		return _data[index * 2 + 1];
	else
		return NULL;
}

/** @fn const char *StringHash::get(const char *key, int item) const
  * Takes the key and item number and concatenates the two into a single
  * string.  Then it calls the internal get function for that single string.
  * @see const char *StringHash::get(const char *bid)
  */
const char* StringHash::get(const char* key, int item) const
{
	//char buf2[strlen(key) + 16];
	const int buff_size = strlen(key) + 16;
	char* buf2 = new char[buff_size];	//ToDo: memory leak !!!
	sprintf_s(buf2, buff_size, "%s%d", key, item);
	return get(buf2);
}

/** @fn int StringHash::get_atoi(const char *key, int item) const
  * Gets the key/value pair associated with the key and item.  If item
  * is set to -1, then it fetches the normal key without the index.
  * Once thats done, returns the integer value (or 0 if its NULL) of the
  * value pointer.
  * @param key An identifier associated with the value we want to find.
  * @param item An item number which is concatenated if applicable to the identifier.
  * @return The value stored with the identifier key/item in integer form.
  */
int StringHash::get_atoi(const char* key, int item) const
{
	const char* value = (item == -1) ? get(key) : get(key, item);
	if (!value) return 0;
	return atoi(value);
}

/// Returns the number of elements in the map.
int StringHash::size() const
{
	return _size;
}


/// Modifies the element associated at index and makes it all lowercase.
void StringHash::lower(int index)
{
	if (index < 0 || index >= _size)
		return;

	for (char* c = _data[index * 2 + 1]; *c; c++)
		*c = tolower(*c);
}

/// Modifies the element associated at index and makes it all uppercase.
void StringHash::upper(int index)
{
	if (index < 0 || index >= _size)
		return;

	for (char* c = _data[index * 2 + 1]; *c; c++)
		*c = toupper(*c);
}

/** @fn bool StringHash::put_a(char * name, char *value)
  * Puts a newly allocated name/value pair (via malloc/strdup) into the map.
  * First, the number of elements is compared to the capacity of the map -- if there
  * isn't space, it reallocates via grow, and if thats unsuccessful, returns false.
  * If there's enough space (or space was allocated to cover the addition) then a linear
  * search is performed on the map to find name.  If name is in the map, then it frees the
  * pointer name and value and returns false.
  * If its not in the map, then the name/value is added to the end of the map and the size
  * is incremented by one.  Returns true in this case.
  *
  * This should not be called by external functions outside the class.
  * @param name The key you wish to enter into the hash map.
  * @param value The value you wish to enter into the hash map.
  * @return true if able to insert successfully, false otherwise
  */
bool StringHash::put_a(char* name, char* value)
{
	if (_size == _capacity && !grow())
		return false;
	for (int i = 0; i < _size; i++)
		if (!strcmp(name, _data[i * 2]))
		{
			free(name); free(value);
			return false;               // already exists, so fail
		}
	_data[_size * 2] = name;
	_data[_size * 2 + 1] = value;
	_size++;
	return true;
}

/** @fn bool StringHash::put(const char *name, const char *value)
  * If value is NULL or points to a string with the first character as NUL, then
  * a copy of both the name and the string "NULL" are inserted it into the map via put_a().
  * Otherwise it copies both the name and the value and inserts it into the map vit put_a().
  * @return Same as put_a.
  * @see bool StringHash::put_a(char *name, char*value)
  */
bool StringHash::put(const char* name, const char* value)
{
	if (value == NULL || *value == 0)
		return put_a(_strdup(name), _strdup("NULL"));
	else
		return put_a(_strdup(name), _strdup(value));
}

/** @fn bool StringHash::grow()
  * Increases the capacity of the map by a factor of two, copies the old contents into the new,
  * and deletes the old, setting this Hash's map to the new one.
  * @return true if successful in allocating memory and copying it over, false if unable to allocate.
  */
bool StringHash::grow()
{
	_capacity += MAGIC_SIZE;
	char** tmp = (char**)malloc(sizeof(char*) * 2 * _capacity);
	if (_data == NULL) return false;
	memcpy(tmp, _data, _size * 2 * sizeof(char*));
	char** tmp2 = _data;
	_data = tmp;
	free(tmp2);
	return true;
}

/** @fn const char * StringHash::getKey(int index) const
  * @return NULL if the bounds are bad, or the key pointer associated with the given index.
  * @param index An index into the map.
  */
const char* StringHash::getKey(int index) const
{
	if (index >= 0 && index < _size)
		return _data[index * 2];
	else
		return NULL;
}

/** @fn const char * StringHash::findKey(const char * value) const
  * @return NULL if the value is null or not found, or the key pointer matching the provided value.
  * @param value A value to find in the map.
  */
const char* StringHash::findKey(const char* value) const
{
	if (!value)
		return NULL;

	char* retString = NULL;
	size_t valueLen = strlen(value);
	for (int idx = 0; !retString && idx < _size; idx++)
	{
		const char* curValue = _data[idx * 2 + 1];
		if (!curValue)
			continue;
		size_t curValueLen = strlen(curValue);
		if ((valueLen == curValueLen) && !strncmp(value, curValue, valueLen))
			retString = _data[idx * 2];
	}
	return retString;
}

/** @fn const char * StringHash::getValue(int index) const
  * @return NULL if the bounds are bad, or the value pointer associated with the given index.
  * @param index An index into the map.
  */
const char* StringHash::getValue(int index) const
{
	if (index >= 0 && index < _size)
		return _data[index * 2 + 1];
	else
		return NULL;
}

/** @fn bool * StringHash::reset()
  * Resets the hash to its original state.  Deallocates the old
  * strings stored in the hash and deallocates the map.  Then it
  * basically does the code for the constructor.
  * @return true if size is 0 or if it can allocate the memory necessary.
  */
bool StringHash::reset()
{
	// dont reset empty hashes
	if (_size == 0) return true;

	// free old data
	for (int i = 0; i < _size; i++) {
		free(_data[i * 2]);
		free(_data[i * 2 + 1]);
	}
	free(_data);

	// init new data
	_data = (char**)malloc(sizeof(char*) * 2 * MAGIC_SIZE);
	bool ok = _data != NULL;
	_size = 0;
	_capacity = ok ? MAGIC_SIZE : 0;

	return ok;
}

/** @fn bool StringHash::remove(int index)
  * Removes the key/value at the given index (no bounds checking or pointer
  * verification is done), decrements the size by one, and moves all the data
  * down by two. (because a key/value takes up two indicies)
  * @return Always returns true.
  */
bool StringHash::remove(int index)
{
	free(_data[index * 2]);
	free(_data[index * 2 + 1]);
	_size--;

	for (int i = index; i < _size; i++) {  // move all the data down a notch
		_data[i * 2] = _data[i * 2 + 2];
		_data[i * 2 + 1] = _data[i * 2 + 3];
	}

	return true;
}

/// Searches the map for the given name and removes it via the other remove function.
bool StringHash::remove(const char* name)
{
	for (int i = 0; i < _size; i++)
		if (!strcmp(name, _data[i * 2]))
			return remove(i);

	return false;
}

bool StringHash::parseXML(const char* xml)
{
	if (!xml || !*xml) return false;

	char tag[256], data[1024];

	while ((xml = strchr(xml, '<')))
	{
		// Don't process close tags
		if (xml[1] == '/') {
			xml++;
			continue;
		}

		// Find the end of the tag name
		const char* end = strchr(xml, '>');
		if (!end) return false;

		size_t len = (size_t)(end - xml) - 1;
		memcpy(tag, xml + 1, len);
		tag[len] = 0;

		xml = end + 1;

		// Find the next tag
		end = strchr(xml, '<');
		if (!end) return false;

		// Is it a close tag?
		if (end[1] != '/')
			xml = end;
		else
			if (strncmp(end + 2, tag, strlen(tag)) == 0)
			{
				// Stuff the intervening characters into the hash
				size_t len = (size_t)(end - xml);
				if (len > (sizeof(data) - 1)) len = sizeof(data) - 1;
				memcpy(data, xml, len);
				data[len] = 0;
				put(tag, data);
				xml = end + 2;
			}

	}

	return true;
}

bool StringHash::parseCGI(const char* cgi)
{
	const char* ptr = cgi;
	char key[256];
	char value[256];

	while (ptr && *ptr)
	{
		int i = 0;
		while (*ptr && *ptr != '=' && i < 255)
			key[i++] = tolower(*ptr++);
		key[i] = 0;
		ptr++;

		i = 0;
		while (*ptr && *ptr != '&' && *ptr != '\n' && i < 255)
			value[i++] = *ptr++;
		value[i] = 0;
		ptr++;

		put(key, value);
	}

	return true;
}


/** @fn bool StringHash::check(const char *key, const char *val) const
  * Fetches the value with the given key.  If value doesn't exist, return
  * false.  Otherwise, do a string compare against val and value.  If
  * they are equal, return true, otherwise return false.
  * @return see Description.
  * @param key The key we want to find.
  * @param val The value we are checking against.
  */
bool StringHash::check(const char* key, const char* val) const
{
	const char* value = get(key);
	return (value && strcmp(val, value) == 0);
}

/** @fn bool StringHash::exists(const char *key) const
  * Determine whether the specified key exists
  * @return bool, whether key exists or not
  * @param key The key we want to find.
  */
bool StringHash::exists(const char* key) const
{
	const char* value = get(key);
	return value != NULL;
}

/** @fn bool StringHash::absent(const char *key) const
  * Determine whether the specified key is absent/missing
  * @return bool, whether key is absent/missing or not
  * @param key The key we want to find.
  */
bool StringHash::absent(const char* key) const
{
	const char* value = get(key);
	return value == NULL;
}


/// Merges the two maps into this map, leaving any values which were originally there intact (in case of with having the same elements)
void StringHash::merge(StringHash& with)
{
	int i;
	for (i = 0; i < with.size(); i++)
		put(with.getKey(i), with.getValue(i));
}

/// A duplicate of StringHash::check(const char *key, const char *val) but with a hardcoded value of "null" instead.
bool StringHash::isNull(const char* key) const
{
	const char* value = get(key);
	return (!value || !strcmp(value, "null"));
}

/// Print out the contents of the hash to a user specified destination (stderr or a file)
void StringHash::dump(FILE* fp, int options) const
{
	int i;
	for (i = 0; i < size(); i++)
	{
		switch (options & 3)
		{
		case 0:
			fprintf(fp, "%s = ", getKey(i));
			break;
		case 1:
			fprintf(fp, "<%s>", getKey(i));
			break;
		case 2:
			fprintf(fp, "[%s,", getKey(i));
			break;
		case 3:
			fprintf(fp, "%s, ", getKey(i));
			break;
		} //switch
		if ((stderr == fp) && (options & 4))
			fprintf(fp, "\033[34m"); //blue text
		fprintf(fp, "%s", getValue(i));
		if ((stderr == fp) && (options & 4))
			fprintf(fp, "\033[0m");
		switch (options & 3)
		{
		case 0:
			fprintf(fp, "\n");
			break;
		case 1:
			fprintf(fp, "</%s>\n", getKey(i));
			break;
		case 2:
			fprintf(fp, "]\n");
			break;
		case 3:
			fprintf(fp, "\n");
			break;
		} //switch
	}
}

bool StringHash::parseTLV1(const char* tlv)
{
	while (true)
	{
		if (!*tlv) return true;
		if (strlen(tlv) < 5) return false;
		char tag[3 + 1] = { tlv[0], tlv[1], tlv[2], 0 };
		char len[2 + 1] = { tlv[3], tlv[4], 0 };
		int ll = 0;
		sscanf_s(len, "%x", &ll);
		if (ll == 0) {
			tlv += 5;
			continue;
		}
		tlv += 5;
		if ((int)strlen(tlv) < ll) return false;
		//char val[ll + 1];
		char* val = new char[ll + 1];
		if (val)
		{
			val[ll] = 0;
			for (int i = 0; i < ll; i++)
				val[i] = tlv[i];
			tlv += ll;
			put(tag, val);
			delete val;
		}
	}
	return true;
}

const std::string StringHash::getStr(const char* key) const
{
	return isNull(key) ? "" : get(key);
}

const std::string StringHash::getStr(const std::string& key) const
{
	return getStr(key.c_str());
}

const std::string StringHash::getStr(const char* key, int item) const
{
	const char* value = get(key, item);
	return value ? value : "";
}

const std::string StringHash::getStr(const std::string& key, int item) const
{
	return getStr(key.c_str(), item);
}

const std::string StringHash::getStringifiedHash(const std::string& separator) const
{
	std::string stringifiedHash = "";

	stringifiedHash.append("BEGIN").append(separator);
	for (int i = 0; i < size(); i++)
	{
		stringifiedHash.append((const char*)getKey(i));
		stringifiedHash.append("=");
		stringifiedHash.append((const char*)getValue(i));
		stringifiedHash.append(separator);
	}
	stringifiedHash.append("END").append(separator);

	return stringifiedHash;
}
