#include <iostream>
#include <iomanip>
#include <rcsvf/reader.hpp>
#include <Rcpp.h>

#include "../inst/csvf/src/reader.cpp"

extern "C"
SEXP rcsvf_reader_create()
{
    //auto fname_cpp = Rcpp::as<std::string>(fname);
    return R_MakeExternalPtr(new rcsvf::reader(), R_NilValue, R_NilValue);
}

extern "C"
void rcsvf_reader_destroy(SEXP ptr)
{
    delete static_cast<rcsvf::reader*>(R_ExternalPtrAddr(ptr));
}

extern "C"
void rcsvf_reader_open(SEXP ptr,
                       SEXP fname,
                       SEXP sep,
                       SEXP eol,
                       SEXP quote_rule,
                       SEXP fill,
                       SEXP strip_white,
                       SEXP skip_blank_lines,
                       SEXP quote,
                       SEXP verbose,
                       SEXP begin_offset,
                       SEXP end_offset)
{
    static_cast<rcsvf::reader*>(R_ExternalPtrAddr(ptr))->open(
        fname, sep, eol, quote_rule, fill, strip_white,
        skip_blank_lines, quote, verbose,
        begin_offset, end_offset);
}

namespace rcsvf {
    reader& reader::open(SEXP fname,
                         SEXP sep,
                         SEXP eol,
                         SEXP quote_rule,
                         SEXP fill,
                         SEXP strip_white,
                         SEXP skip_blank_lines,
                         SEXP quote,
                         SEXP verbose,
                         SEXP begin_offset,
                         SEXP end_offset)
    {
        //auto fname_cpp = Rcpp::as<std::string>(fname);
        std::string fname_cpp = CHAR(STRING_ELT(fname,0));
        m_filename = fname_cpp;
        m_file.close();
        m_file.open(fname_cpp);

        m_sep = Rcpp::as<std::string>(sep)[0];
        m_eol.clear();
        std::string eol_cpp = CHAR(STRING_ELT(eol,0));
        m_eol.resize(eol_cpp.size());
        std::copy(eol_cpp.begin(), eol_cpp.end(), m_eol.begin());

        auto quote_rule_cpp = Rcpp::as<std::string>(quote_rule);
        if (quote_rule_cpp == "doubled") {
            m_quote_rule = quote_rule_type::doubled;
        } else if (quote_rule_cpp == "escaped") {
            m_quote_rule = quote_rule_type::escaped;
        } else if (quote_rule_cpp == "verbatim") {
            m_quote_rule = quote_rule_type::verbatim;
        } else if (quote_rule_cpp == "none") {
            m_quote_rule = quote_rule_type::none;
        } else {
            Rf_error("invalid value for argument quote_rule");
        }
        
        m_fill = Rf_asLogical(fill) > 0;

        m_strip_white = Rf_asLogical(strip_white) > 0;

        m_skip_blank_lines = Rf_asLogical(skip_blank_lines) > 0;

        m_quote = Rcpp::as<std::string>(quote)[0];

        m_verbose = Rf_asLogical(verbose) > 0;

        ptrdiff_t begin_offset_cpp=-1, end_offset_cpp=-1;
        if (!Rf_isNull(begin_offset)) {
            begin_offset_cpp = reinterpret_cast<ptrdiff_t>(R_ExternalPtrAddr(begin_offset));
        }
        if (!Rf_isNull(end_offset)) {
            end_offset_cpp = reinterpret_cast<ptrdiff_t>(R_ExternalPtrAddr(end_offset));
        }
        
        m_nfields = -1;

        m_begin = m_file.data();
        m_end = m_file.data() + m_file.size();
        strip_if_bom();
        detect_eol();
        strip_space();
        detect_sep_quote_rule_nfields();

        if (begin_offset > 0) {
            m_begin = m_file.data() + begin_offset_cpp;
        }

        if (end_offset > 0) {
            m_end = m_file.data() + end_offset_cpp;
        }
        return *this;
    }
    
    reader& reader::detect_field_types()
    {
        const char *pos_original = m_pos;
        
        m_field_types.resize(m_nfields);
        std::fill(m_field_types.begin(), m_field_types.end(), field_type::STRING);
        
        // sample a certain number of records
        const int nsamples = 20;
        std::set<const char*> starts;

        for (int i=0; i<20; i++) {
            const char *m_pos = m_begin + (m_end-m_begin)/(nsamples+1);
            anywhere_to_next_record_begin();
            starts.insert(m_pos);
        }

        std::vector<const char*> starts2(starts.begin(), starts.end());

        // try to determine field types
        int nsamples_actual = starts2.size();
        if (nsamples_actual == 1 && starts2[0] == m_end) {
            // the file contains header or one record only
            // so use the beginning as record start
            starts2[0] = m_begin;
        }

        for (auto pos : starts2) {
            m_pos = pos;
            auto record = read_record();
            assert(record.size()==m_nfields);
            for (int j=0; j<m_nfields; j++) {
                // remove quote if it is
                auto field = record[j];
                if (field.front()=='"' && field.back()=='"') {
                    field.remove_prefix(1);
                    field.remove_suffix(1);
                }
                std::string field2(field.begin(), field.end());

                // check if convertible to integer
                bool sucess = true;
                try {
                    std::stoll(field2);
                } catch(...) {
                    sucess = false;
                }
                if (sucess) {
                    m_field_types[j] = field_type::INTEGER;
                    continue;
                }

                // check if convertible to double
                sucess = true;
                try {
                    std::stold(field2);
                } catch(...) {
                    sucess = false;
                }
                if (sucess) {
                    m_field_types[j] = field_type::DOUBLE;
                    continue;
                }
            }
        }

        m_pos = pos_original;
        return *this;
    }
}



