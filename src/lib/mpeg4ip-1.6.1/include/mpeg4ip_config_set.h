/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is MPEG4IP.
 * 
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2000, 2001.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Dave Mackie		dmackie@cisco.com
 *		Bill May 		wmay@cisco.com
 */

#ifndef __CONFIG_SET_H__
#define __CONFIG_SET_H__

#include <mpeg4ip.h>


#ifndef CONFIG_SAFETY
#define CONFIG_SAFETY 1
#endif

typedef u_int32_t config_integer_t;

typedef u_int32_t config_index_t;


enum ConfigException {
	CONFIG_ERR_INAME,
	CONFIG_ERR_TYPE,
	CONFIG_ERR_MEMORY,
};

// TBD type specific exception info and printing utility
class CConfigException {
public:
	CConfigException(ConfigException e) {
		type = e;
	}
	ConfigException	type;
};

#define CONFIG_MAX_STRLEN	255

// TBD weld this in, and throw exception

typedef enum ConfigType {
	CONFIG_TYPE_UNDEFINED,
	CONFIG_TYPE_INTEGER,
	CONFIG_TYPE_BOOL,
	CONFIG_TYPE_STRING,
	CONFIG_TYPE_FLOAT
} ConfigType;

union UConfigValue {
	UConfigValue(void) {
		m_svalue = NULL;
	}
	UConfigValue(config_integer_t ivalue) {
		m_ivalue = ivalue;
	}
	UConfigValue(bool bvalue) {
		m_bvalue = bvalue;
	}
  UConfigValue(const char *svalue) {
    m_svalue = svalue;
  }
	UConfigValue(char* svalue) {
		m_svalue = svalue;
	}
	UConfigValue(float fvalue) {
		m_fvalue = fvalue;
	}

	config_integer_t	m_ivalue;
	bool			m_bvalue;
	const char*		m_svalue;
	float				m_fvalue;
};

typedef struct SConfigVariable {
        config_index_t	               *m_iName;
	const char* 			m_sName;
	ConfigType			m_type;
	UConfigValue			m_defaultValue;
	const char*                     m_helpString;
	UConfigValue			m_value;
} SConfigVarible;



struct SUnknownConfigVariable {
  struct SUnknownConfigVariable *next;
  SConfigVariable *value;
};

class CConfigSet {
public:
	CConfigSet(const SConfigVariable* variables, 
	  config_index_t numVariables, 
	  const char* defaultFileName) {
	  uint32_t size;
		m_debug = false;
		m_numVariables = numVariables;
		size = sizeof(SConfigVariable) * numVariables;
		m_variables = 
		  (SConfigVariable*)malloc(size);

		memcpy(m_variables, variables, size);
		m_fileName = defaultFileName == NULL ? NULL : 
		  strdup(defaultFileName);
		SetToDefaults();
		m_unknown_head = NULL;
	};

	~CConfigSet() {
		CHECK_AND_FREE(m_fileName);
		for (config_index_t i = 0; i < m_numVariables; i++) {
		  CleanUpConfig(&m_variables[i]);
		}
		free(m_variables);
		m_variables = NULL;
		SUnknownConfigVariable *ptr = m_unknown_head;
		while (ptr != NULL) {
		  m_unknown_head = ptr->next;
		  CHECK_AND_FREE(ptr->value->m_sName);
		  CHECK_AND_FREE(ptr->value->m_value.m_svalue);
		  free(ptr->value);
		  free(ptr);
		  ptr = m_unknown_head;
		}
	}

	void InitializeIndexes(void) {
	  for (config_index_t ix = 0; ix < m_numVariables; ix++) {
	      *m_variables[ix].m_iName = ix;
	    }
	}

	void AddConfigVariables (const SConfigVariable* vars,
				 config_index_t numVariables) {
	  config_index_t start = m_numVariables;
	  uint32_t size = sizeof(SConfigVariable) * 
	    (m_numVariables + numVariables);
	  m_variables = (SConfigVariable*)realloc(m_variables, size);
	  memcpy(&m_variables[m_numVariables], vars, 
		 numVariables * sizeof(SConfigVariable));
	  m_numVariables += numVariables;
	  SetToDefaults(start);
	}

	const char* GetFileName() {
		return m_fileName;
	}

	void SetFileName(const char *name) {
	  CHECK_AND_FREE(m_fileName);
	  m_fileName = strdup(name);
	};
	
	inline void CheckIName(config_index_t iName) {
		if (iName >= m_numVariables) {
		  fprintf(stderr, 
			  "config variable index failure - try %u max %u\n",
			  iName, m_numVariables);
		  fflush(stderr);
		  throw new CConfigException(CONFIG_ERR_INAME);
		}
		if (*m_variables[iName].m_iName != iName) {
		  fprintf(stderr, 
			   "config variable index mismatch %u should %u\n",
			   iName, *m_variables[iName].m_iName);
		  fflush(stderr);
		  throw new CConfigException(CONFIG_ERR_INAME);
		}
	}


	inline void CheckIntegerType(config_index_t iName) {
		if (m_variables[iName].m_type != CONFIG_TYPE_INTEGER) {
		  fprintf(stderr, 
			  "config type mismatch on %s - shouldn't be integer\n", 
			  m_variables[iName].m_sName);
		  fflush(stderr);
		  throw new CConfigException(CONFIG_ERR_TYPE);
		}
	}

	inline void CheckBoolType(config_index_t iName) {
		if (m_variables[iName].m_type != CONFIG_TYPE_BOOL) {
		  fprintf(stderr, 
			  "config type mismatch on %s - shouldn't be bool\n", 
			  m_variables[iName].m_sName);
		  fflush(stderr);
		  throw new CConfigException(CONFIG_ERR_TYPE);
		}
	}

	inline void CheckStringType(config_index_t iName) {
		if (m_variables[iName].m_type != CONFIG_TYPE_STRING) {
		  fprintf(stderr, 
			  "config type mismatch on %s - shouldn't be string\n", 
			  m_variables[iName].m_sName);
		  fflush(stderr);
			throw new CConfigException(CONFIG_ERR_TYPE);
		}
	}

	inline void CheckFloatType(config_index_t iName) {
		if (m_variables[iName].m_type != CONFIG_TYPE_FLOAT) {
		  fprintf(stderr, 
			  "config type mismatch on %s - shouldn't be float\n", 
			  m_variables[iName].m_sName);
		  fflush(stderr);
			throw new CConfigException(CONFIG_ERR_TYPE);
		}
	}

	inline bool IsDefault (const config_index_t iName) {
#if CONFIG_SAFETY
	  CheckIName(iName);
#endif
	  return IsValueDefault(&m_variables[iName]);
	};

	inline config_integer_t GetIntegerValue(const config_index_t iName) {
#if CONFIG_SAFETY
		CheckIName(iName);
		CheckIntegerType(iName);
#endif
		return m_variables[iName].m_value.m_ivalue;
	}

	inline void SetIntegerValue(const config_index_t iName, 
	  config_integer_t ivalue) {
#if CONFIG_SAFETY
		CheckIName(iName);
		CheckIntegerType(iName);
#endif
		m_variables[iName].m_value.m_ivalue = ivalue;
	}

	inline bool GetBoolValue(const config_index_t iName) {
#if CONFIG_SAFETY
		CheckIName(iName);
		CheckBoolType(iName);
#endif
		return m_variables[iName].m_value.m_bvalue;;
	}

	inline void SetBoolValue(const config_index_t iName, bool bvalue) {
#if CONFIG_SAFETY
		CheckIName(iName);
		CheckBoolType(iName);
#endif
		m_variables[iName].m_value.m_bvalue = bvalue;
	}

	inline const char* GetStringValue(const config_index_t iName) {
#if CONFIG_SAFETY
		CheckIName(iName);
		CheckStringType(iName);
#endif
		return m_variables[iName].m_value.m_svalue;
	}

	inline const char* GetHelpValue(const config_index_t iName) {
#if CONFIG_SAFETY
	  CheckIName(iName);
#endif
	  return m_variables[iName].m_helpString;
	};

	inline void SetStringValue(const config_index_t iName, const char* svalue) {
#if CONFIG_SAFETY
		CheckIName(iName);
		CheckStringType(iName);
#endif
		if (svalue == m_variables[iName].m_value.m_svalue) {
			return;
		}
		CHECK_AND_FREE(m_variables[iName].m_value.m_svalue);
		if (svalue == NULL) {
		  m_variables[iName].m_value.m_svalue = NULL;
		} else {
		  m_variables[iName].m_value.m_svalue = strdup(svalue);
		  if (m_variables[iName].m_value.m_svalue == NULL) {
		    throw new CConfigException(CONFIG_ERR_MEMORY);
		  }
		}
	}

	inline float GetFloatValue(const config_index_t iName) {
#if CONFIG_SAFETY
		CheckIName(iName);
		CheckFloatType(iName);
#endif
		return m_variables[iName].m_value.m_fvalue;
	}

	inline void SetFloatValue(const config_index_t iName, float fvalue) {
#if CONFIG_SAFETY
		CheckIName(iName);
		CheckFloatType(iName);
#endif
		m_variables[iName].m_value.m_fvalue = fvalue;
	}

	void SetToDefaults(int start = 0) {
		for (config_index_t i = start; i < m_numVariables; i++) {
			SetToDefault(&m_variables[i]);
		}
	}

	void SetToDefault(const config_index_t iName) {
	  SetToDefault(&m_variables[iName]);
	}

	void ProcessLine (char *line) {
	// comment
	  SConfigVariable* var;
	  if (line[0] == '#') {
	    return;
	  }
	  char* s = line, *name;
	  while (*s != '\0') s++;
	  s--;
	  while (isspace(*s)) {
	    *s = '\0';
	    s--;
	  }
	  s = name = line;

	  var = FindByName(strsep(&s, "="));
	  if (var == NULL || s == NULL) {
	    if (s != NULL) {
	      SConfigVariable *svar;
	      svar = MALLOC_STRUCTURE(SConfigVariable);
	      memset(svar, 0, sizeof(*svar));
	      svar->m_sName = strdup(name);
	      svar->m_type = CONFIG_TYPE_STRING;
	      if (!FromAscii(svar, s)) {
		fprintf(stderr, "bad config value in line %s\n", s);
	      }
	      SUnknownConfigVariable *ptr;
	      ptr = MALLOC_STRUCTURE(SUnknownConfigVariable);
	      ptr->next = m_unknown_head;
	      ptr->value = svar;
	      m_unknown_head = ptr;
	    }
	    if (m_debug) {
	      fprintf(stderr, "bad config line %s\n", s);  
	    }
	    return;
	  }
	  if (!FromAscii(var, s)) {
	    if (m_debug) {
	      fprintf(stderr, "bad config value in line %s\n", s);  
	    }
	  }
	}

	void SetVariableFromAscii(config_index_t ix, char *arg) {
	  FromAscii(&m_variables[ix], arg);
	};

	bool ReadFile(const char* fileName = NULL) {
	  if (fileName == NULL && m_fileName == NULL) 
	    return false;
       
	  if (fileName != NULL) {
		CHECK_AND_FREE(m_fileName);
		m_fileName = strdup(fileName);
	  }
		FILE* pFile = fopen(m_fileName, "r");
		if (pFile == NULL) {
			if (m_debug) {
				fprintf(stderr, "couldn't open file %s\n", fileName);
			}
			return false;
		}
		char line[256];
		while (fgets(line, sizeof(line), pFile)) {
		  ProcessLine(line);
		}
		fclose(pFile);
		return true;
	}

	bool WriteToFile(const char* fileName = NULL, bool allValues = false) {
		FILE* pFile;
		config_index_t i;
		SConfigVariable *var;
		SUnknownConfigVariable *ptr;
		if (fileName == NULL && m_fileName == NULL) {
		  return false;
		}
		if (fileName == NULL) {
		  fileName = m_fileName;
		}
		pFile = fopen(fileName, "w");
		if (pFile == NULL) {
			if (m_debug) {
				fprintf(stderr, "couldn't open file %s\n", fileName);
			}
			return false;
		}
		for (i = 0; i < m_numVariables; i++) {
			var = &m_variables[i];
			if (allValues || !IsValueDefault(var)) {
				fprintf(pFile, "%s=%s\n", var->m_sName, 
					ToAscii(var));
			}
		}
		ptr = m_unknown_head;
		while (ptr != NULL) {
		  fprintf(pFile, "%s=%s\n", ptr->value->m_sName, 
			  ToAscii(ptr->value));
		  ptr = ptr->next;
		}
		fclose(pFile);
		return true;
	}


	void SetDebug(bool debug = true) {
		m_debug = debug;
	}
#ifdef _WIN32
	int ReadVariablesFromRegistry(const char *reg_name, 
				      const char *config_section);
	void WriteVariablesToRegistry(const char *reg_name, 
				      const char *config_section);
#endif
 public:
	config_index_t FindIndexByName(const char *sName) {
		for (config_index_t i = 0; i < m_numVariables; i++) {
			if (!strcasecmp(sName, m_variables[i].m_sName)) {
			  return i;
			}
		}
		return UINT32_MAX;
	}
	ConfigType GetTypeFromIndex (config_index_t index) {
	  if (index < m_numVariables) {
	    return m_variables[index].m_type;
	  }
	  return CONFIG_TYPE_UNDEFINED;
	};
	const char *GetNameFromIndex (config_index_t index) {
	  if (index < m_numVariables) {
	    return m_variables[index].m_sName;
	  }
	  return NULL;
	};
	config_index_t GetNumVariables (void) {
	  return m_numVariables;
	};
	void DisplayHelp(bool onlyHelp = false) {
	  config_index_t ix;
	  for (ix = 0; ix < m_numVariables; ix++) {
	    if (m_variables[ix].m_helpString) {
	      fprintf(stdout, "%s - %s\n", m_variables[ix].m_sName,
		      m_variables[ix].m_helpString);
	    } else if (onlyHelp == false) {
	      fprintf(stdout, "%s\n", m_variables[ix].m_sName);
	    }
	  }
	};
	void Dump (void) {
	  fprintf(stdout, "File - %s\n", GetFileName());
	  config_index_t ix;
	  for (ix = 0; ix < m_numVariables; ix++) {
	    fprintf(stdout, "%s:\t%s\n", 
		    m_variables[ix].m_sName, 
		    ToAscii(&m_variables[ix]));
	  }
	};
	const char* GetUnknownStringValue (const char *var_name) {
	  SUnknownConfigVariable *svar = m_unknown_head;
	  while (svar != NULL) {
	    if (strcasecmp(var_name, svar->value->m_sName) == 0) {
	      return svar->value->m_value.m_svalue;
	    }
	    svar = svar->next;
	  }
	  config_index_t ix = FindIndexByName(var_name);
	  if (ix == UINT32_MAX) return NULL;
	  return m_variables[ix].m_value.m_svalue;
	};
	
protected:
	SConfigVariable* FindByName(const char* sName) {
	  config_index_t ix = FindIndexByName(sName);
	  if (ix == UINT32_MAX) return NULL;
	  return &m_variables[ix];
	};

protected:
	SConfigVariable*	m_variables;
	config_index_t		m_numVariables;
	bool 			m_debug;
	const char*		m_fileName;
	SUnknownConfigVariable *m_unknown_head;
	const char* ToAscii(SConfigVariable *v) {
	  static char sBuf[CONFIG_MAX_STRLEN+3];
	  switch (v->m_type) {
	  case CONFIG_TYPE_INTEGER:
	    sprintf(sBuf, "%d", v->m_value.m_ivalue);
	    return sBuf;
	  case CONFIG_TYPE_BOOL:
	    sprintf(sBuf, "%d", v->m_value.m_bvalue);
	    return sBuf;
	  case CONFIG_TYPE_STRING:
	    if (v->m_value.m_svalue == NULL) {
	      sprintf(sBuf, "\"\"");
	      return sBuf;
	    } 
	    if (strchr(v->m_value.m_svalue, ' ')) {
	      sBuf[0] = '"';
	      strncpy(&sBuf[1], v->m_value.m_svalue, CONFIG_MAX_STRLEN);
	      strcpy(&sBuf[MIN(strlen(v->m_value.m_svalue), 
			       CONFIG_MAX_STRLEN)+1], "\"");
	    }
	    return v->m_value.m_svalue;
	  case CONFIG_TYPE_FLOAT:
	    sprintf(sBuf, "%f", v->m_value.m_fvalue);
	    return sBuf;
	  case CONFIG_TYPE_UNDEFINED:
	  default:
	    return "";
	  }
	};

	bool FromAscii(SConfigVarible *v, const char* s) {
	  switch (v->m_type) {
	  case CONFIG_TYPE_INTEGER:
	    return (sscanf(s, " %i ", &v->m_value.m_ivalue) == 1);
	  case CONFIG_TYPE_BOOL:
	    // OPTION could add "yes/no", "true/false"
	    if (sscanf(s, " %u ", &v->m_value.m_ivalue) != 1) {
	      return false;
	    }
	    v->m_value.m_bvalue = v->m_value.m_ivalue ? true : false;
	    return true;
	  case CONFIG_TYPE_STRING:
	    // N.B. assuming m_svalue has been alloc'ed
	    {
	      size_t len = strlen(s);
	      CHECK_AND_FREE(v->m_value.m_svalue);
	      if (*s == '"' && s[len] == '"') {
		char *newvalue = strdup(s + 1);
		newvalue[len - 1] = '\0';

		v->m_value.m_svalue = newvalue;
	      } else {
		v->m_value.m_svalue = strdup(s);
	      }
	      if (v->m_value.m_svalue == NULL) {
		throw new CConfigException(CONFIG_ERR_MEMORY);
	      }
	      return true;
	    }
	  case CONFIG_TYPE_FLOAT:
	    return (sscanf(s, " %f ", &v->m_value.m_fvalue) == 1);
	  case CONFIG_TYPE_UNDEFINED:
	  default:
	    return false;
	  }
	};

	void SetToDefault(SConfigVariable *v) {
	  switch (v->m_type) {
	  case CONFIG_TYPE_INTEGER:
	    v->m_value.m_ivalue = v->m_defaultValue.m_ivalue;
	    break;
	  case CONFIG_TYPE_BOOL:
	    v->m_value.m_bvalue = v->m_defaultValue.m_bvalue;
	    break;
	  case CONFIG_TYPE_STRING:
	    CHECK_AND_FREE(v->m_value.m_svalue);
	    if (v->m_defaultValue.m_svalue == NULL) {
	      v->m_value.m_svalue = NULL;
	    } else {
	      v->m_value.m_svalue = strdup(v->m_defaultValue.m_svalue);
	      if (v->m_value.m_svalue == NULL) {
		throw new CConfigException(CONFIG_ERR_MEMORY);
	      }
	    }
	    break;
	  case CONFIG_TYPE_FLOAT:
	    v->m_value.m_fvalue = v->m_defaultValue.m_fvalue;
	    break;
	  case CONFIG_TYPE_UNDEFINED:
	  default:
	    break;
	  } 
	};

	bool IsValueDefault(SConfigVarible *v) {
	  switch (v->m_type) {
	  case CONFIG_TYPE_INTEGER:
	    return v->m_value.m_ivalue == v->m_defaultValue.m_ivalue;
	  case CONFIG_TYPE_BOOL:
	    return v->m_value.m_bvalue == v->m_defaultValue.m_bvalue;
	  case CONFIG_TYPE_STRING:
	    if (v->m_defaultValue.m_svalue == NULL && 
		v->m_value.m_svalue == NULL) 
	      return true;
	    if (v->m_defaultValue.m_svalue == NULL) return false;
	    if (v->m_value.m_svalue == NULL) return false;
	    return (strcmp(v->m_value.m_svalue, 
			   v->m_defaultValue.m_svalue) == 0);
	  case CONFIG_TYPE_FLOAT:
	    return v->m_value.m_fvalue == v->m_defaultValue.m_fvalue;
	  case CONFIG_TYPE_UNDEFINED:
	  default:
	    return false;
	  } 
	};
        void CleanUpConfig(SConfigVariable *v) {
	  if (v->m_type == CONFIG_TYPE_STRING) {
	    CHECK_AND_FREE(v->m_value.m_svalue);
	  }
	};
};

// To define configuration variables - first DECLARE_CONFIG in a
// .h file.  Then in either a C++ or h file, define a static array
// of configuration variables using CONFIG_BOOL, CONFIG_FLOAT, CONFIG_INT
// or CONFIG_STRING.  You can include the .h anywhere you use the variable - 
// in a .cpp, you must include the .h file with DECLARE_CONFIG_VARIABLES
// defined before the .h file.  Note - if you're already including mp4live.h, 
// you need to #define the DECLARE_CONFIG_VARIABLES after the include.
//
// Note - you want to add the config variables BEFORE the ReadFile
// call

#define CONFIG_BOOL(var, name, defval) \
 { &(var), (name), CONFIG_TYPE_BOOL, (defval), }
#define CONFIG_FLOAT(var, name, defval) \
 { &(var), (name), CONFIG_TYPE_FLOAT,(float) (defval), }
#define CONFIG_INT(var, name, defval) \
 { &(var), (name), CONFIG_TYPE_INTEGER,(config_integer_t) (defval), }
#define CONFIG_STRING(var, name, defval) \
 { &(var), (name), CONFIG_TYPE_STRING, (const char *)(defval), (const char *)NULL }
#define CONFIG_BOOL_HELP(var, name, defval, help) \
 { &(var), (name), CONFIG_TYPE_BOOL, (defval), (help), }
#define CONFIG_FLOAT_HELP(var, name, defval, help) \
 { &(var), (name), CONFIG_TYPE_FLOAT,(float) (defval), (help), }
#define CONFIG_INT_HELP(var, name, defval, help) \
 { &(var), (name), CONFIG_TYPE_INTEGER,(config_integer_t) (defval), (help), }
#define CONFIG_STRING_HELP(var, name, defval, help) \
 { &(var), (name), CONFIG_TYPE_STRING, (const char *)(defval), (help), (const char *)NULL,  }


#endif /* __CONFIG_SET_H__ */

#undef DECLARE_CONFIG
#ifdef DECLARE_CONFIG_VARIABLES
#define DECLARE_CONFIG(a) config_index_t (a);
#else
#define DECLARE_CONFIG(a) extern config_index_t (a);
#endif
