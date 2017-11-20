#include <string>
#include <csvf/reader.hpp>
#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace rcsvf {
    class reader : public csvf::reader {
    public:
        enum class field_type {INTEGER, DOUBLE, STRING};
        enum class header_type {YES, NO, AUTO};
        
        /** @name Constructors and destructors
         */
        /**@{*/
        /** default constructor */
        reader() = default;
        /** virtual destructor */
        virtual ~reader() {};

        // /**
        //  * construct a reader object from a file this constructor will
        //  * call member function open() to open the file specified by
        //  * fname
        //  *
        //  * @param fname a string specifies the path of the csv file
        //  * @args other arguments will pass to open
        //  *
        //  */
        // template <typename... Args>
        // explicit reader(const std::string& fname, Args... args)
        // {
        //     open(fname, args...);
        // }

        /**
         * open a csv file
         *
         * @param fname a string specifies the path of the csv file
         * @args options in string-value pairs
         *
         * @example 
         */
        template <typename... Args>
        reader& open(const std::string& fname, Args... args)
        {
            m_field_types.clear();
            m_field_names.clear();
            
            ptrdiff_t begin_offset=-1, end_offset=-1;
            m_header = header_type::AUTO;
            auto tmpopts = options::parse(args...);
            options::assign(tmpopts,
                            "begin_offset", begin_offset,
                            "end_offset", end_offset,
                            "header", m_header);

            // override begin_offset and end_offset
            csvf::reader::open(
                fname, args...,
                "begin_offset", static_cast<ptrdiff_t>(-1),
                "end_offset", static_cast<ptrdiff_t>(-1));
            
            detect_field_types();
            detect_field_names();

            // for (int i=0; i<5; i++) {
            //     Rprintf("i=%d,",i);
            //     Rprintf("field=%s,",i, m_field_names[i].c_str());
            // }
            // Rprintf("\n");
                
            
            if (begin_offset >= 0) {
                m_begin = m_file.data() + begin_offset;
                if (m_pos < m_begin) {
                    m_pos = m_begin;
                }
            }

            if (end_offset >= 0) {
                m_end = m_file.data() + end_offset;
                if (m_pos < m_begin) {
                    m_pos = m_begin;
                }
            }

            return *this;
        }
            
        reader& detect_field_types();

        reader& detect_field_names();

        std::vector<field_type> field_types() const
        {
            return m_field_types;
        }

        std::vector<std::string> field_names() const
        {
            return m_field_names;
        }

    protected:
        std::vector<field_type> m_field_types;
        std::vector<std::string> m_field_names;
        header_type m_header;
    };
}
