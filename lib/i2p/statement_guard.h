#include <sqlite3cc/basic_statement.h>

namespace i2pcpp {
    class statement_guard {
        boost::shared_ptr<sqlite::detail::basic_statement> m_statement;

        void bind() {}

        template<typename T>
            void bind(T&& t)
            {
                *m_statement << t;
            }

        template<typename T, typename ...U>
            void bind(T&& t, U&& ...u)
            {
                *m_statement << t;
                bind(u...);
            }

        public:
        template<typename ...Params>
            statement_guard(boost::shared_ptr<sqlite::detail::basic_statement> s,
                    Params&&... p) : m_statement(s)
        {
            bind(std::forward<Params>(p)...);
        }

        ~statement_guard()
        {
            m_statement->clear_bindings();
            m_statement->reset();
        }
    };
}
