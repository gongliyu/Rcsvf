#include <string>
#include <csvf/reader.hpp>
#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace rcsvf {
    class reader : public csvf::reader {
    public:
        enum class field_type {UNKNOWN,INTEGER, DOUBLE, STRING};
        
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
        reader& open(SEXP fname,
                     SEXP sep,
                     SEXP eol,
                     SEXP quote_rule,
                     SEXP fill,
                     SEXP strip_white,
                     SEXP skip_blank_lines,
                     SEXP quote,
                     SEXP verbose,
                     SEXP begin_offset,
                     SEXP end_offset);
        
        reader& detect_field_types();

        std::vector<field_type> field_types() const
        {
            return m_field_types;
        }

    protected:
        std::vector<field_type> m_field_types;
    };
}
