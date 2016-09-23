// Copyright 2016 Chirstopher Torres (Raven), L3nn0x
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * \file cmysql_database
 *
 * \author L3nn0x
 * \date march 2016
 *
 * The MySQL implementation of \s IDatabase, \s IResult and \s IRow
 * \sa IDatabase IResult IRow
 */
#ifndef _CMYSQL_DATABASE_H_
#define _CMYSQL_DATABASE_H_

#include "idatabase.h"
#include <mysql++.h>
#include <mutex>
#include "logconsole.h"

namespace Core {
/*!
 * \class CMySQL_Row
 *
 * \brief The MySQL implementation of \s IRow
 * \sa IRow CMySQL_Result IResult CMySQL_Database IDatabase
 *
 * \author L3nn0x
 * \date march 2016
 */
class CMySQL_Row : public IRow {
	public:
		CMySQL_Row(const mysqlpp::Row &row) : row_(row) {}
		virtual ~CMySQL_Row() {}

		virtual bool getString(std::string const &columnName, std::string &data);
		virtual bool getInt(std::string const &columnName, uint32_t &data);
		virtual bool getFloat(std::string const &columnName, float &data);

	private:
		mysqlpp::Row	row_;

		template <typename T>
		bool getData(std::string const &name, T &data) {
			auto tmp = row_[name.c_str()];
			if (tmp.is_null()) return false;
			data = static_cast<T>(tmp);
			return true;
		}
};

/*!
 * \class CMySQL_Result
 *
 * \brief The MySQL implementation of \s IResult
 * \sa IRow CMySQL_Row IResult CMySQL_Database IDatabase
 *
 * \author L3nn0x
 * \date march 2016
 */
class CMySQL_Result : public IResult {
 public:
  CMySQL_Result(const mysqlpp::StoreQueryResult&);
  virtual ~CMySQL_Result() {}

  virtual bool 		incrementRow();
  virtual uint32_t	size() const {return rows_.size();}

  virtual bool getString(std::string const &columnName, std::string &data);
  virtual bool getInt(std::string const &columnName, uint32_t &data);
  virtual bool getFloat(std::string const &columnName, float &data);
};

/*!
 * \class CMySQL_Database
 *
 * \brief The MySQL implementation of \s IDatabase
 * \sa IRow CMySQL_Row IResult CMySQL_Result IDatabase
 *
 * \author L3nn0x
 * \date march 2016
 */
class CMySQL_Database : public IDatabase {
 public:
  CMySQL_Database();
  CMySQL_Database(const std::string &_host, const std::string &_database,
				  const std::string &_user, const std::string &_password);
  virtual ~CMySQL_Database();

  virtual void Connect(const std::string &_host, const std::string &_database,
                       const std::string &_user, const std::string &_password);

  virtual void QExecute(const std::string &_query);
  virtual std::unique_ptr<IResult> QStore(const std::string &_query);

  /*!
   * \brief This function escapes a string to make it MySQL safe.
   *
   * \param[in] data The data to escape
   * \return a safe escaped copy of the input
   */
  static std::string escapeData(const std::string &data);

 private:
  std::string hostname_;
  std::string database_;
  std::string username_;
  std::string password_;

  std::mutex mutex_;
  mysqlpp::Connection conn_;
  bool connected_;
  std::weak_ptr<spdlog::logger> logger_;
};
}

#endif
