#ifndef _STRINGHASH_H
#define _STRINGHASH_H

#include <string>

class StringHash
{
public:
	StringHash();
	~StringHash();

	/** @fn bool parseXML(const char *xml)
	  * Parses the parameters of a string containing xml and flatten
	  * it into this map.
	  * @param xml The string containing the xml elements.
	  * @return false if there was an error, true otherwise.
	  */
	bool parseXML(const char* xml);
	// Parse an XML string and push the parameters into the hash.
	// Returns false if there was an error parsing.

/** @fn bool parseCGI(const char *cgi)
  * Parses the parameters of a URL encoded string and puts it into
  * the map.
  * @param cgi The string containing the CGI elements (e.g: foo=x&goo=y)
  * @return false if there was an error, true otherwise.
  */
	bool parseCGI(const char* cgi);

	/** @fn bool parseTLV(const char *tlv)
	  * Parses a TLV and places the values into the hash.
	  * TLV Format: TTTLLV.. where TTT is the three character Tag,
	  * LL is the two hex length (in bytes), and V... is the value.
	  * @param tlv the string containing the tlv.
	  * @return false if there was an error, true otherwise.
	  */
	bool parseTLV1(const char* tlv);

	// Parse a string containing CGI parameters and place the values into the hash.
	// Returns false if there was an error parsing.

	bool remove(int);
	bool remove(const char*);
	bool remove(const std::string& name) { return remove(name.c_str()); }
	bool reset();
	const char* getKey(int) const;
	const char* findKey(const char* value) const;
	const char* getValue(int) const;
	enum { MAGIC_SIZE = 128 };
	bool put(const char*, const char*);
	bool put(const std::string& name, const std::string& value) { return put(name.c_str(), value.c_str()); }
	bool put_a(char*, char*);
	int size() const;
	void lower(int index);
	void upper(int index);
	const char* get(int) const;
	const char* get(const char*) const;

	const char* get(const char*);
	// TODO Deprecated - there are libraries that still require this signature
	// i.e. libfraud.so

	const char* get(const char* key, int item) const;
	// Convenience access to fields with the "productcode#" format

	int get_atoi(const char* key, int item = -1) const;
	// Convenience access to numeric fields, returns a 0 on null.

	bool check(const char* key, const char* val) const;
	// Returns true if key exists and its value matches val.

	void merge(StringHash& with);
	// Merge "with" into this hash; it will NOT overwrite existing data with the same key

	bool isNull(const char* key) const;
	bool isNull(const std::string& key) const { return isNull(key.c_str()); }
	bool exists(const char* key) const;
	bool absent(const char* key) const;

	// Returns true if key does not exist or is equal to "null"
	void dump(FILE* fp, int options = 0) const;

	/** @fn const string getStr(const string& key)
	  * Get the value of the specified hash key.
	  * @param key - hash key name
	  * @return If the specified key exists, return the associated value; otherwise,
	  *		an empty string is returned.
	  */
	const std::string getStr(const std::string& key) const;

	/** @fn const string getStr(const char *key)
	  * Get the value of the specified hash key.
	  * @param key - hash key name
	  * @return If the specified key exists, return the associated value; otherwise,
	  *		an empty string is returned.
	  */
	const std::string getStr(const char* key) const;

	/** @fn const string getStr(const string& key, int item)
	  * Get the value of the specified hash key and item number.
	  * @param key - hash key name
	  * @param item - number to be concatenated to key's value
	  * @return If the specified key exists, return the associated value; otherwise,
	  *		an empty string is returned.
	  */
	const std::string getStr(const std::string& key, int item) const;

	/** @fn const string getStr(const char *key, int item)
	  * Get the value of the specified hash key and item number.
	  * @param key - hash key name
	  * @param item - number to be concatenated to key's value
	  * @return If the specified key exists, return the associated value; otherwise,
	  *		an empty string is returned.
	  */
	const std::string getStr(const char* key, int item) const;

	/** @fn const std::string getStringifiedHash(const std::string& separator = "\n")
	  * Get a TC Link equivalent representation of key value pairs stored in the hash.
	  * Output format: BEGIN\nkey1=val1\nkey=val2\n.....,keyN=valN\nEND\n
	  * @param separator - separator string used for key value pair separation (default "\n")
	  * @return a string containing TC Link equivalent representation of key value pairs
	  */
	const std::string getStringifiedHash(const std::string& separator = "\n") const;

protected:
	bool grow();
	char** _data;
	int _size;
	int _capacity;
};

#endif	// _STRINGHASH_H
