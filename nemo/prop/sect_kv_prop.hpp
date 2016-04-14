/*
 * sect_kv_prop.hpp
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */

#ifndef SECT_KV_PROP_HPP_
#define SECT_KV_PROP_HPP_

#include <nemo/util.hpp>

#include <map>
#include <string>

namespace nemo
{

namespace prop
{

class sect_kv_prop
{
public:
    const static char CONF_COMMENT_CHAR = '#';
    const static char CONF_SECTION_START = '[';
    const static char CONF_SECTION_END = ']';

    typedef std::map<std::string, std::map<std::string, std::string> > prop_t;

public:
    sect_kv_prop(const std::string &cnf_file_name, bool trim = true, char separator = '=') :
            m_cnf_file_name(cnf_file_name), m_is_trim(trim), m_separator(separator)
    {
        init();
    }

    ~sect_kv_prop(void)
    {
        m_conf.clear();
        m_cnf_file_name.clear();
        m_separator = '=';
    }

    std::string
    get_conf_value_str(const std::string& section_name, const std::string& param_key, const std::string& default_val) const
    {
        std::string value = default_val;
        std::map<std::string, std::map<std::string, std::string> >::const_iterator ite;
        ite = m_conf.find(section_name);
        if (ite != m_conf.end())
        {
            std::map<std::string, std::string> temp_map = ite->second;
            std::map<std::string, std::string>::const_iterator temp_ite;
            temp_ite = temp_map.find(param_key);
            if (temp_ite != temp_map.end())
            {
                value = temp_ite->second;
            }
        }
        return value;
    }

    void
    dump_properties(std::ostream &os)
    {
        prop_t::const_iterator ite1 = m_conf.begin();
        while (ite1 != m_conf.end())
        {
            os << "[" << ite1->first << "]" << std::endl;
            std::map<std::string, std::string>::const_iterator ite2 = ite1->second.begin();
            while (ite2 != ite1->second.end())
            {
                os << ite2->first << "=" << ite2->second << std::endl;
                ite2++;
            }
            ite1++;
        }
    }

protected:
    void
    init(void)
    {
        std::ifstream file;
        file.open(m_cnf_file_name.c_str(), std::ios::in);

        if (!file)
        {
            return;
        }

        std::string section("*");
        char line[2048];
        size_t leading_pos;

        while (!file.eof())
        {
            file.getline(line, sizeof(line));
            std::string tmp_str(line);
            leading_pos = tmp_str.find_first_not_of(" \t", 0);

            if (leading_pos == tmp_str.npos)
            { // empty line
                continue;
            }
            else if (tmp_str[leading_pos] == '#')
            { // comment
                continue;
            }
            else if (tmp_str[leading_pos] == '[')
            { // section
                nemo::util::str::trim(tmp_str);
                for (size_t ind = 0; (ind + 1) < tmp_str.size(); ind++)
                {
                    if (line[ind + 1] == ']')
                    {
                        line[ind] = 0;
                        break;
                    }
                    else
                    {
                        line[ind] = line[ind + 1];
                    }
                }
                section = line;
                if (m_conf.find(section) == m_conf.end())
                {
                    m_conf.insert(
                            std::map<std::string,
                                    std::map<std::string, std::string> >::value_type(
                                    section,
                                    std::map<std::string, std::string>()));
                }
                continue;
            }
            else
            {

                size_t separatorPos = 0;
                separatorPos = tmp_str.find_first_of(m_separator);

                std::string key, value;
                if (separatorPos != tmp_str.npos && separatorPos < tmp_str.length())
                {
                    key.assign(tmp_str.begin(), tmp_str.begin() + separatorPos);
                    value.assign(tmp_str.begin() + separatorPos + 1, tmp_str.end());
                }
                else
                { //this line has no separator
                    key = line;
                    value = "";
                }
                if (m_is_trim)
                {
                    nemo::util::str::trim(key);
                    nemo::util::str::trim(value);
                }

                std::map<std::string, std::map<std::string, std::string> >::iterator tmp_itr_sec =
                        m_conf.find(section);
                if (tmp_itr_sec != m_conf.end())
                {
                    std::map<std::string, std::string>::iterator tmp_itr_key =
                            tmp_itr_sec->second.find(key);
                    if (tmp_itr_key == tmp_itr_sec->second.end())
                    {
                        tmp_itr_sec->second.insert(
                                std::map<std::string, std::string>::value_type(
                                        key, value));
                    }
                    else
                    {
                        tmp_itr_key->second = value;
                    }
                }
            }
        }

        file.close();
    }

protected:
    std::map<std::string, std::map<std::string, std::string> > m_conf;
    std::string m_cnf_file_name;
    bool m_is_trim;
    char m_separator;

};

}

}

#endif /* SECT_KV_PROP_HPP_ */
