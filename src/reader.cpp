#include <iostream>
#include <iomanip>
#include <cstdint>
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
                       SEXP end_offset,
                       SEXP header)
{
    // ptr
    auto ptr_cpp = static_cast<rcsvf::reader*>(R_ExternalPtrAddr(ptr));

    // fname
    std::string fname_cpp = CHAR(STRING_ELT(fname,0));

    // sep
    std::string tmpstr = CHAR(STRING_ELT(sep,0));
    char sep_cpp = tmpstr[0];

    // eol
    std::vector<char> eol_cpp(0);
    if (Rf_isNull(eol)) {
        eol_cpp.clear();
    } else {
        tmpstr = CHAR(STRING_ELT(eol,0));
        eol_cpp.resize(tmpstr.size());
        std::copy(tmpstr.begin(), tmpstr.end(), eol_cpp.begin());
    }

    // quote_rule
    tmpstr = CHAR(STRING_ELT(quote_rule,0));
    rcsvf::reader::quote_rule_type quote_rule_cpp;
    if (tmpstr == "doubled") {
        quote_rule_cpp = rcsvf::reader::quote_rule_type::doubled;
    } else if (tmpstr == "escaped") {
        quote_rule_cpp = rcsvf::reader::quote_rule_type::escaped;
    } else if (tmpstr == "verbatim") {
        quote_rule_cpp = rcsvf::reader::quote_rule_type::verbatim;
    } else if (tmpstr == "none") {
        quote_rule_cpp = rcsvf::reader::quote_rule_type::verbatim;
    } else if (tmpstr == "auto.detect") {
        quote_rule_cpp = rcsvf::reader::quote_rule_type::auto_detect;
    } else {
        Rf_error("invalid value for argument quote_rule");
    }

    // fill
    bool fill_cpp = Rf_asLogical(fill) > 0;
    
    // strip_white
    bool strip_white_cpp = Rf_asLogical(strip_white) > 0;

    // skip_blank_lines
    bool skip_blank_lines_cpp = Rf_asLogical(skip_blank_lines)>0;

    // quote
    tmpstr = CHAR(STRING_ELT(quote,0));
    char quote_cpp = tmpstr[0];

    // verbose
    bool verbose_cpp = Rf_asLogical(verbose) > 0;

    // begin_offset
    ptrdiff_t begin_offset_cpp = -1;
    if (!Rf_isNull(begin_offset)) {
        double tmp = Rf_asReal(begin_offset);
        int64_t tmp2 = reinterpret_cast<int64_t&>(tmp);
        begin_offset_cpp = static_cast<ptrdiff_t>(tmp2);
    }

    // end_offset
    ptrdiff_t end_offset_cpp = -1;
    if (!Rf_isNull(end_offset)) {
        double tmp = Rf_asReal(end_offset);
        int64_t tmp2 = reinterpret_cast<int64_t&>(tmp);
        end_offset_cpp = static_cast<ptrdiff_t>(tmp2);
    }

    // header
    rcsvf::reader::header_type header_cpp;
    if (Rf_isLogical(header)) {
        header_cpp = Rf_asLogical(header) > 0 ?
            rcsvf::reader::header_type::YES :
            rcsvf::reader::header_type::NO;
    } else if (Rf_isString(header) &&
               std::strcmp(CHAR(STRING_ELT(header,0)),"auto")==0) {
        header_cpp = rcsvf::reader::header_type::AUTO;
    } else {
        Rf_error("option \"header\" should be a logical value or"
                 "\"auto\"");
    }
        
    
    ptr_cpp->open(fname_cpp,
                  "sep", sep_cpp,
                  "eol", eol_cpp,
                  "quote_rule", quote_rule_cpp,
                  "fill", fill_cpp,
                  "strip_white", strip_white_cpp,
                  "skip_blank_lines", skip_blank_lines_cpp,
                  "quote", quote_cpp,
                  "verbose", verbose_cpp,
                  "begin_offset", begin_offset_cpp,
                  "end_offset", end_offset_cpp,
                  "header", header_cpp);
}

/**
 * query properties of the reader object
 */
extern "C"
SEXP rcsvf_reader_properties(SEXP ptr)
{
    rcsvf::reader* ptr_cpp = static_cast<rcsvf::reader*>(R_ExternalPtrAddr(ptr));
    
    std::vector<const char*> names{"sep", "eol", "quote.rule", "fill",
            "strip.white", "skip.blank.lines",
            "quote", "verbose", "nfields", "field.types",
            "field.names"};
    R_xlen_t N = names.size();
    SEXP nms = Rf_protect(Rf_allocVector(STRSXP, N));
    for (int i=0; i<N; i++){
        SET_STRING_ELT(nms, i, Rf_mkChar(names[i]));
    }
    SEXP props = Rf_protect(Rf_allocVector(VECSXP, N));    
    Rf_setAttrib(props, R_NamesSymbol, nms);

    // sep
    std::string sep_cpp(1, ptr_cpp->sep());
    SEXP sep = Rf_protect(Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(sep, 0, Rf_mkChar(sep_cpp.c_str()));
    SET_VECTOR_ELT(props, 0, sep);

    // eol
    std::vector<char> eol_vec_cpp = ptr_cpp->eol();
    std::string eol_str_cpp(eol_vec_cpp.begin(), eol_vec_cpp.end());
    SEXP eol = Rf_protect(Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(eol, 0, Rf_mkChar(eol_str_cpp.c_str()));
    SET_VECTOR_ELT(props, 1, eol);

    // quote rule
    auto quote_rule_cpp = ptr_cpp->quote_rule();
    SEXP quote_rule = Rf_protect(Rf_allocVector(STRSXP, 1));
    switch (quote_rule_cpp)
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
    case rcsvf::reader::quote_rule_type::auto_detect:
        SET_STRING_ELT(quote_rule, 0, Rf_mkChar("auto.detect"));
        break;
    }
    SET_VECTOR_ELT(props, 2, quote_rule);

    // fill
    SEXP fill = Rf_protect(Rf_ScalarLogical(ptr_cpp->fill()));
    SET_VECTOR_ELT(props, 3, fill);

    // strip.white
    SEXP strip_white = Rf_protect(Rf_ScalarLogical(ptr_cpp->strip_white()));
    SET_VECTOR_ELT(props, 4, strip_white);

    // skip.blank.lines
    SEXP skip_blank_lines = Rf_protect(Rf_ScalarLogical(ptr_cpp->skip_blank_lines()));
    SET_VECTOR_ELT(props, 5, skip_blank_lines);

    // quote
    std::string quote_cpp(1, ptr_cpp->quote());
    SEXP quote = Rf_protect(Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(quote, 0, Rf_mkChar(quote_cpp.c_str()));
    SET_VECTOR_ELT(props, 6, quote);

    // verbose
    SEXP verbose = Rf_protect(Rf_ScalarLogical(ptr_cpp->verbose()));
    SET_VECTOR_ELT(props, 7, verbose);

    // nfields
    SEXP nfields = Rf_protect(Rf_ScalarInteger(ptr_cpp->nfields()));
    SET_VECTOR_ELT(props, 8, nfields);

    // field types
    auto field_types_cpp = ptr_cpp->field_types();
    SEXP field_types = Rf_protect(Rf_allocVector(STRSXP, field_types_cpp.size()));
    for (size_t i=0; i<field_types_cpp.size(); i++) {
        switch (field_types_cpp[i])
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

    // field names
    auto field_names_cpp = ptr_cpp->field_names();
    SEXP field_names = Rf_protect(Rf_allocVector(STRSXP, field_names_cpp.size()));
    for (size_t i=0; i<field_names_cpp.size(); i++) {
        SET_STRING_ELT(field_names, i, Rf_mkChar(field_names_cpp[i].c_str()));
    }
    SET_VECTOR_ELT(props, 10, field_names);
    
    Rf_unprotect(13);
    return props;
}

/**
 * read a specified number of records
 */
extern "C"
SEXP rcsvf_reader_read(SEXP ptr, SEXP n)
{
    auto ptr_cpp = static_cast<rcsvf::reader*>(R_ExternalPtrAddr(ptr));
    int nfields = ptr_cpp->nfields();
    auto n_cpp = Rf_asInteger(n);
    auto field_types = ptr_cpp->field_types();
    auto field_names = ptr_cpp->field_names();

    // allocate result list
    SEXP res = Rf_protect(Rf_allocVector(VECSXP, nfields));
    std::vector<void*> field_addr(nfields, nullptr);
    SEXP nms = Rf_protect(Rf_allocVector(STRSXP, nfields));
    for (int i=0; i<nfields; i++) {
        SET_STRING_ELT(nms, i, Rf_mkChar(field_names[i].c_str()));
        switch (field_types[i])
        {
        case rcsvf::reader::field_type::STRING:
            SET_VECTOR_ELT(res, i, Rf_allocVector(STRSXP, n_cpp));
            field_addr[i] = VECTOR_ELT(res, i);
            break;
            
        case rcsvf::reader::field_type::DOUBLE:
            SET_VECTOR_ELT(res, i, Rf_allocVector(REALSXP, n_cpp));
            field_addr[i] = REAL(VECTOR_ELT(res, i));
            break;
            
        case rcsvf::reader::field_type::INTEGER:
            SET_VECTOR_ELT(res, i, Rf_allocVector(INTSXP, n_cpp));
            field_addr[i] = INTEGER(VECTOR_ELT(res, i));
            break;
        default:
            Rf_error("unhandled field type in rcsvf_reader_read");
        }
    }
    Rf_setAttrib(res, R_NamesSymbol, nms);
    SEXP rownames = Rf_protect(Rf_allocVector(INTSXP, n_cpp));
    for (int i=0; i<n_cpp; i++) {
        INTEGER(rownames)[i] = i+1;
    }

    //read n records
    int nread = 0;
    while (nread < n_cpp && *ptr_cpp) {
        auto record = ptr_cpp->read_record();
        for (int j=0; j<nfields; j++) {
            switch (field_types[j])
            {
            case rcsvf::reader::field_type::STRING:
                SET_STRING_ELT(
                    static_cast<SEXP>(field_addr[j]), nread,
                    Rf_mkChar(static_cast<std::string>(record[j]).c_str()));
                break;
            case rcsvf::reader::field_type::DOUBLE:
                static_cast<double*>(field_addr[j])[nread]=std::stold(static_cast<std::string>(record[j]));
                break;
            case rcsvf::reader::field_type::INTEGER:
                static_cast<int*>(field_addr[j])[nread]=std::stoll(static_cast<std::string>(record[j]));
                break;
            default:
                Rf_error("unhandled field type in rcsvf_reader_read");
            }
        }
        nread++;
    }

    for (int i=0; i<nfields; i++) {
        SETLENGTH(VECTOR_ELT(res,i), nread);
    }
    SETLENGTH(rownames, nread);
    
    Rf_setAttrib(res, R_RowNamesSymbol, rownames);
    Rf_setAttrib(res, R_ClassSymbol, Rf_mkString("data.frame"));
    Rf_unprotect(3);
    return res;
}

/**
 * estimate positions for equal chunks
 */
extern "C"
SEXP rcsvf_reader_chunk_uniformly(SEXP ptr, SEXP nchunks, SEXP npositions, SEXP nrecords_per_position)
{
    // ptr
    auto ptr_cpp = static_cast<rcsvf::reader*>(R_ExternalPtrAddr(ptr));

    // nchunks
    int nchunks_cpp = Rf_asInteger(nchunks);
    
    // npositions
    int npositions_cpp = Rf_asInteger(npositions);

    // nrecords_per_position
    int nrecords_per_position_cpp = Rf_asInteger(nrecords_per_position);

    double nrecords_cpp = 0;
    std::vector<ptrdiff_t> offsets_cpp = ptr_cpp->chunk_uniformly(nrecords_cpp, nchunks_cpp, npositions_cpp, nrecords_per_position_cpp);

    SEXP offsets = Rf_protect(Rf_allocVector(REALSXP, offsets_cpp.size()));
    for (int i=0; i<offsets_cpp.size(); i++) {
        // ptrdiff_t is a 64-bits integer on 64 bit platform. However,
        // the integer value in R is 32 bit. So we need to store its
        // bit-values in a double value.
        int64_t tmp = static_cast<int64_t>(offsets_cpp[i]);
        REAL(offsets)[i] = reinterpret_cast<double&>(tmp);
    }

    const char *names[] = {"offsets", "estimated_total_nrecords",""};
    SEXP res = Rf_protect(Rf_mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, offsets);
    SET_VECTOR_ELT(res, 1, Rf_ScalarReal(nrecords_cpp));
    
    Rf_unprotect(2);
    return res;
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
                // remove quote if it is
                auto field = record[j];
                if (field.front()=='"' && field.back()=='"') {
                    field.remove_prefix(1);
                    field.remove_suffix(1);
                }
                std::string field2(field.begin(), field.end());

                // check if convertible to integer
                if (m_field_types[j] == field_type::INTEGER) {
                    bool success = check_integer(field2);
                    if (!success) {
                        m_field_types[j] = check_double(field2) ? field_type::DOUBLE : field_type::STRING;
                    }
                } else if (m_field_types[j] == field_type::DOUBLE) {
                    if (!check_double(field2)) {
                        m_field_types[j] = field_type::STRING;
                    }
                }
            }
        }

        m_pos = pos_original;
        return *this;
    }

    reader& reader::detect_field_names()
    {
        const char *pos_original = m_pos;
        m_field_names.resize(m_nfields);
        for (int i=0; i<m_nfields; i++) {
            std::ostringstream stream;
            stream<<"V"<<i+1;
            m_field_names[i] = stream.str();
        }

        if (m_header == header_type::YES) {
            auto record = csvf::reader::read_record();
            for (int i=0; i<m_nfields; i++) {
                if (!record[i].empty()) {
                    m_field_names[i] = static_cast<std::string>(record[i]);
                }
            }
        } else if (m_header == header_type::AUTO) {
            // if the first row can not convert to the types detected,
            // then it will be treated as header
            const char *pos = m_pos;
            auto record = csvf::reader::read_record();
            bool success = true;
            for (int i=0; i<m_nfields; i++) {
                switch (m_field_types[i])
                {
                case field_type::INTEGER:
                    success = check_integer(static_cast<std::string>(record[i]));
                    break;

                case field_type::DOUBLE:
                    success = check_double(static_cast<std::string>(record[i]));
                    break;

                case field_type::STRING:
                    break;
                }
                if (!success) {
                    break;
                }
            }

            if (!success) {
                for (int i=0; i<m_nfields; i++) {
                    m_field_names[i] = static_cast<std::string>(record[i]);
                }
            } else {
                m_pos = pos_original;
            }
        }
        
        return *this;
    }
    
}



