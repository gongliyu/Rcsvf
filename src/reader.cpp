#include <iostream>
#include <iomanip>
#include <rcsvf/reader.hpp>
#include <Rcpp.h>

#include "../inst/csvf/src/reader.cpp"

/**
 * create reader object
 */
extern "C"
SEXP rcsvf_reader_create()
{
    return R_MakeExternalPtr(new rcsvf::reader(), R_NilValue, R_NilValue);
}

/**
 * destroy reader object
 */
extern "C"
void rcsvf_reader_destroy(SEXP ptr)
{
    delete static_cast<rcsvf::reader*>(R_ExternalPtrAddr(ptr));
}

/**
 * open csv file
 */
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

/**
 * query properties of the reader object
 */
extern "C"
SEXP rcsvf_reader_properties(SEXP ptr)
{
    rcsvf::reader* cpp_ptr = static_cast<rcsvf::reader*>(R_ExternalPtrAddr(ptr));
    

    std::vector<const char*> names{"sep", "eol", "quote.rule", "fill",
            "strip.white", "skip.blank.lines",
            "quote", "verbose", "nfields", "field.types"};
    R_xlen_t N = names.size();
    SEXP nms = Rf_protect(Rf_allocVector(STRSXP, N));
    for (int i=0; i<N; i++){
        SET_STRING_ELT(nms, i, Rf_mkChar(names[i]));
    }
    SEXP props = Rf_protect(Rf_allocVector(VECSXP, N));    
    Rf_setAttrib(props, R_NamesSymbol, nms);

    // sep
    std::string cpp_sep(1, cpp_ptr->sep());
    SEXP sep = Rf_protect(Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(sep, 0, Rf_mkChar(cpp_sep.c_str()));
    SET_VECTOR_ELT(props, 0, sep);

    // eol
    std::vector<char> cpp_eol_vec = cpp_ptr->eol();
    std::string cpp_eol_str(cpp_eol_vec.begin(), cpp_eol_vec.end());
    SEXP eol = Rf_protect(Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(eol, 0, Rf_mkChar(cpp_eol_str.c_str()));
    SET_VECTOR_ELT(props, 1, eol);

    // quote rule
    auto cpp_quote_rule = cpp_ptr->quote_rule();
    SEXP quote_rule = Rf_protect(Rf_allocVector(STRSXP, 1));
    switch (cpp_quote_rule)
    {
    case rcsvf::reader::quote_rule_type::doubled:
        SET_STRING_ELT(quote_rule, 0, Rf_mkChar("doubled"));
        break;
    case rcsvf::reader::quote_rule_type::escaped:
        SET_STRING_ELT(quote_rule, 0, Rf_mkChar("escaped"));
        break;
    case rcsvf::reader::quote_rule_type::verbatim:
        SET_STRING_ELT(quote_rule, 0, Rf_mkChar("verbatim"));
        break;
    case rcsvf::reader::quote_rule_type::none:
        SET_STRING_ELT(quote_rule, 0, Rf_mkChar("none"));
        break;
    }
    SET_VECTOR_ELT(props, 2, quote_rule);

    // fill
    SEXP fill = Rf_protect(Rf_ScalarLogical(cpp_ptr->fill()));
    SET_VECTOR_ELT(props, 3, fill);

    // strip.white
    SEXP strip_white = Rf_protect(Rf_ScalarLogical(cpp_ptr->strip_white()));
    SET_VECTOR_ELT(props, 4, strip_white);

    // skip.blank.lines
    SEXP skip_blank_lines = Rf_protect(Rf_ScalarLogical(cpp_ptr->skip_blank_lines()));
    SET_VECTOR_ELT(props, 5, skip_blank_lines);

    // quote
    std::string cpp_quote(1, cpp_ptr->quote());
    SEXP quote = Rf_protect(Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(quote, 0, Rf_mkChar(cpp_quote.c_str()));
    SET_VECTOR_ELT(props, 6, quote);

    // verbose
    SEXP verbose = Rf_protect(Rf_ScalarLogical(cpp_ptr->verbose()));
    SET_VECTOR_ELT(props, 7, verbose);

    // nfields
    SEXP nfields = Rf_protect(Rf_ScalarInteger(cpp_ptr->nfields()));
    SET_VECTOR_ELT(props, 8, nfields);

    // field types
    auto cpp_field_types = cpp_ptr->field_types();
    SEXP field_types = Rf_protect(Rf_allocVector(STRSXP, cpp_field_types.size()));
    for (size_t i=0; i<cpp_field_types.size(); i++) {
        switch (cpp_field_types[i])
        {
        case rcsvf::reader::field_type::INTEGER:
            SET_STRING_ELT(field_types, i, Rf_mkChar("integer"));
            break;
        case rcsvf::reader::field_type::DOUBLE:
            SET_STRING_ELT(field_types, i, Rf_mkChar("double"));
            break;
        case rcsvf::reader::field_type::STRING:
            SET_STRING_ELT(field_types, i, Rf_mkChar("character"));
            break;
        }
    }
    SET_VECTOR_ELT(props, 9, field_types);
    
    Rf_unprotect(12);
    return props;
}

namespace {
    bool check_integer(const std::string& s)
    {
        if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

        char * p ;
        strtol(s.c_str(), &p, 10) ;

        return (*p == 0) ;
    }

    bool check_double(const std::string& field)
    {
        bool sucess = true;
        try {
            std::stold(field);
        } catch (...) {
            sucess = false;
        }
        return sucess;
    }
}


//************************************************************
// Implementation of rcsvf::reader class
//************************************************************
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
        detect_field_types();

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
        std::fill(m_field_types.begin(), m_field_types.end(), field_type::INTEGER);
        
        // sample a certain number of records
        const int nsamples = 20;
        std::set<const char*> starts;

        for (int i=1; i<=nsamples; i++) {
            m_pos = m_begin + i*(m_end-m_begin)/(nsamples+1);
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
            if (is_end()) m_pos = m_begin;
            auto record = read_record();
            assert(record.size()==m_nfields);
            for (int j=0; j<m_nfields; j++) {
                std::cout<<"j="<<j<<std::endl;
                // remove quote if it is
                auto field = record[j];
                if (field.front()=='"' && field.back()=='"') {
                    field.remove_prefix(1);
                    field.remove_suffix(1);
                }
                std::string field2(field.begin(), field.end());

                std::cout<<"field2="<<field2<<std::endl;
                // check if convertible to integer
                if (m_field_types[j] == field_type::INTEGER) {
                    bool success = check_integer(field2);
                    if (!success) {
                        std::cout<<"check integer failed"<<std::endl;
                        m_field_types[j] = check_double(field2) ? field_type::DOUBLE : field_type::STRING;
                    }
                } else if (m_field_types[j] == field_type::DOUBLE) {
                    std::cout<<"currently double"<<std::endl;
                    if (!check_double(field2)) {
                        m_field_types[j] = field_type::STRING;
                    }
                }
            }
        }

        m_pos = pos_original;
        return *this;
    }
}



