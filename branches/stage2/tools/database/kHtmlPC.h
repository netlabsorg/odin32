/* $Id: kHtmlPC.h,v 1.2 2000-02-18 12:42:08 bird Exp $ */
/*
 * kHtmlPC - Special-purpose HTML/SQL preprocessor.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _kHtmlPC_h_
#define _kHtmlPC_h_

    #define VER_MAJOR  0
    #define VER_MINOR  1

    #pragma pack()


    typedef struct _options
    {
        char *pszBaseDir;
    } OPTIONS, *POPTIONS;


    /**
     * A base class for all LIFO entries.
     * @author      knut st. osmundsen
     */
    class kLIFOEntry
    {
        private:
            kLIFOEntry *pNext; /* private next pointer */

        public:
            kLIFOEntry() : pNext(NULL)           { }
            kLIFOEntry *getNext() const          { return pNext; }
            void setNext(kLIFOEntry *pEntry)     { pNext = pEntry; }
            virtual BOOL operator ==(const char *psz) const = 0;
    };


    /**
     * Error class.
     * @purpose     Hold a enum for errorcodes and a function which convert errorcodes to text.
     * @author      knut st. osmundsen
     */
    class kError
    {
        private:
            static const char *formatMessage(const char *pszMsg);

        public:
            enum enmErrors
            {
                no_error = 0,
                /* errors */
                error_unexpected_end_of_string,
                error_unexpected_eot,
                error_invalid_tag_start_char,
                error_invalid_tagname,
                error_invalid_tag,
                error_no_filename,
                error_unexpected_eof,
                error_unbalanced_quotes,
                error_eot_not_found,
                error_opening_file,
                error_determing_file_size,
                error_new_failed,
                error_reading_file,
                error_incorrect_number_of_parameters,
                error_missing_sql,
                error_invalid_sql_tag,
                error_opening_output_file,
                error_graph_to_small,
                error_graph_type_must_be_specified_before_data,
                error_data_param_is_missing_value,
                error_type_param_is_missing_value,
                error_invalid_type,
                error_filename_param_is_missing_value,
                error_width_cx_param_is_missing_value,
                error_height_cy_param_is_missing_value,
                error_graph_type_is_missing,
                error_graph_subtype_is_invalid,
                error_filename_is_missing,
                error_no_data,
                error_invalid_dimentions,
                error_data_param_is_missing_sql_statement,
                error_xstart_param_is_missing_value,
                error_xend_param_is_missing_value,
                error_ystart_param_is_missing_value,
                error_yend_param_is_missing_value,
                error_sql_failed,

                /* warnings */
                warning_illegal_string_char = 0x8000,
                warning_too_many_parameters,
                warning_unexpected_end_of_string,
                warning_unexpected_eot,
                warning_failed_to_open_background_image,
                warning_failed_to_load_background_image,
                warning_invalid_sub_type,
                warning_title_param_is_missing_value,
                warning_titlex_param_is_missing_value,
                warning_titley_param_is_missing_value,
                warning_invalid_parameter,
                warning_legend_is_not_implemented_yet,
                warning_failed_to_convert_date,
                warning_invalid_color_value,
                warning_to_many_fields_in_data_sql,
                warning_pie_not_implemented_yet,
                warning_negative_values_are_not_supported_yet,
                warning_background_param_is_missing_value,
                warning_backgroundcolor_is_invalid,
                warning_backgroundcolor_param_is_missing_value,
                warning_foregroundcolor_is_invalid,
                warning_foregroundcolor_param_is_missing_value,
                warning_axiscolor_is_invalid,
                warning_axiscolor_param_is_missing_value
            };
            static const char *queryDescription(kError::enmErrors enmErrorCd);
    };


    /**
     * Warning Entry, for use with a list template.
     * @author      knut st. osmundsen
     */
    class kWarningEntry : public kListEntry
    {
        private:
            kError::enmErrors enmErrorCd;

        public:
            kWarningEntry(kError::enmErrors enmErrorCd) : enmErrorCd(enmErrorCd) {};
            kError::enmErrors getWarningCode(void)  { return enmErrorCd; }

            /* these are here because I didn't have time to implement the kList class */
            BOOL operator==(const kWarningEntry &entry) const { return FALSE || ! &entry; } /* "|| ..." due to unref param */
            BOOL operator!=(const kWarningEntry &entry) const { return TRUE  ||   &entry; }
            BOOL operator< (const kWarningEntry &entry) const { return FALSE || ! &entry; }
            BOOL operator<=(const kWarningEntry &entry) const { return FALSE || ! &entry; }
            BOOL operator> (const kWarningEntry &entry) const { return TRUE  ||   &entry; }
            BOOL operator>=(const kWarningEntry &entry) const { return TRUE  ||   &entry; }
    };


    #define TAG_BUFFER_SIZE       1024
    #define TAG_MAX_PARAMETERS      32
    /**
     * Html Tag class.
     * @purpose     Analyse an HTML tag.
     * @author      knut st. osmundsen
     */
    class kTag
    {
        private:
            /**@cat Misc data */
            char           szTag[TAG_BUFFER_SIZE];

            /**@cat Tag data - Tagname */
            const char    *pszTagname;

            /**@cat Tag data - Parameters */
            const char    *apszParameters[TAG_MAX_PARAMETERS];
            const char    *apszValues[TAG_MAX_PARAMETERS];
            unsigned long  cParameters;

            /**@cat Warning */
            kError::enmErrors enmWarning;

            /**@cat Internal functions */
            void           analyseTag(const char *pszTag) throw(kError::enmErrors);
            const char    *copyTag(char *pszTo, const char *pszFrom) throw(kError::enmErrors);
            const char    *copyParameterName(char *pszTo, const char *pszFrom) throw(kError::enmErrors);
            const char    *copyParameterValue1(char *pszTo, const char *pszFrom) throw(kError::enmErrors);
            const char    *copyParameterValue2(char *pszTo, const char *pszFrom) throw(kError::enmErrors);

        public:
            kTag(const char *pszTag) throw(kError::enmErrors);
            ~kTag(void);

            /**@cat Gets/Queries */
            unsigned long  getParameterCount(void) const;
            const char    *getTagname(void) const;
            const char    *queryParameter(const char *pszParameter) const;
            BOOL           queryExists(const char *pszParameter) const;
            const char    *queryParameterName(unsigned long ulOrdinal) const;
            const char    *queryParameterValue(unsigned long ulOrdinal) const;
            BOOL           isTag(const char *pszTagname) const;

            /**@cat Warning */
            const char         *queryWarning(void);
    };


    /**
     * A HTML input file.
     * @author      knut st. osmundsen
     */
    class kFileEntry : public kLIFOEntry
    {
        private:
            char                szFilename[CCHMAXPATH];
            char               *pszFile;
            unsigned long       cbFile;
            const kFileEntry   *pParent;   /* TODO */
            const char         *pszParent; /* TODO */

        protected:
            void openAndReadFile(const char *pszFilename) throw (kError::enmErrors);

        public:
            kFileEntry(const char *pszFilename) throw (kError::enmErrors);
            kFileEntry(const kTag &tag, const char *pszParent, const kFileEntry *pParent) throw (kError::enmErrors);
            virtual ~kFileEntry();

            long         getLineNumber(void) const;
            const char  *getFilename(void) const            { return &szFilename[0]; }
            virtual BOOL operator ==(const char *psz) const { return stricmp(psz, szFilename) == 0; }
            const char  *queryParentPointer(void) const     { return pszParent; }
            const kFileEntry *queryParent(void) const       { return pParent; }

            /** Current pointer */
            const char  *pszCurrent;
    };


    /**
     * Variable Entry.
     * @author      knut st. osmundsen
     */
    class kVariableEntry : public kLIFOEntry
    {
        private:
            char  *pszName;
            char  *pszValue;

        public:
            kVariableEntry(const char *pszName, const char *pszValue);
            virtual ~kVariableEntry();

            const char *getValue(void) const                { return pszValue; }
            virtual BOOL operator ==(const char *psz) const { return stricmp(psz, pszName) == 0; }
    };


    /**
     * Simple LIFO2 template class.
     * Node class must implement:
     *      Node *getNext();
     *      void  setNext(Node*);
     *      BOOL operator ==(const char *psz) const;
     *      find(const char *key);
     *      const VariableEntry *find(const char *pszKey) const;
     *      BOOL                 exists(const VariableEntry *pVariableEntry) const;
     * @purpose     LIFO for SqlEntry objects.
     * @author      knut st. osmundsen
     */
    template <class kEntry, class kEntryEntry>
    class kLIFO2 : public kLIFO<kEntry>
    {
        public:
            virtual const kEntryEntry *findSub(const char *pszKey) const;
            virtual BOOL               existsSub(const kEntryEntry *pEntryEntry) const;
    };


    /**
     * A Sql statement with variables.
     * @author      knut st. osmundsen
     */
    class kSqlEntry : public kLIFOEntry
    {
        private:
            kLIFO<kVariableEntry>  lifoVariables;
            char                  *pszSql;     /* Pointer to sql expression. */
            void                  *pres;       /* MYSQL_RES pointer  */
            unsigned long          cRows;      /* Number of rows left in the result. */
            const kFileEntry      *pFileEntry; /* Pointer to file node which we are to backtrack to. */
            const char            *pszCurrent; /* Pointer to where in the FileEntry we are to backtrack to. */

        protected:
            void dbExecuteQuery(void) throw (kError::enmErrors);
            BOOL dbFetch(void);
            void dbFreeResult(void);

        public:
            kSqlEntry(); /* dummy */
            kSqlEntry(const kTag &tag, const char *pszCurrent, const kFileEntry *pFileEntry) throw (kError::enmErrors);
            virtual ~kSqlEntry();

            virtual BOOL operator ==(const char *psz) const;

            /** additional functions */
            BOOL                  eof(void) const               { return cRows == 0; } /** end of fetch */
            BOOL                  fetch(void);
            const kVariableEntry *find(const char *pszKey) const;
            BOOL                  exists(const kVariableEntry *pVariableEntry) const;

            const kFileEntry     *queryFileEntry(void) const    { return pFileEntry; }
            const char           *queryBackTrackPos(void) const { return pszCurrent; }
            static const char    *querySqlLastError(void);
    };


    /**
     * Graph Data. One coordinate.
     * @author      knut st. osmundsen
     */
    class kGraphData : public kListEntry
    {
        public:
            double rdX;
            double rdY;
            char   szXDate[11];

            BOOL operator==(const kGraphData &entry) const;
            BOOL operator!=(const kGraphData &entry) const;
            BOOL operator< (const kGraphData &entry) const;
            BOOL operator<=(const kGraphData &entry) const;
            BOOL operator> (const kGraphData &entry) const;
            BOOL operator>=(const kGraphData &entry) const;
    };


    /**
     * Graph Dataset. One set of data.
     * @author      knut st. osmundsen
     */
    class kGraphDataSet : public kListEntry
    {
        public:
            kSortedList<kGraphData>  listData;    /* lines  */
            double         rdCount;               /* pie    */
            unsigned long  ulColor;
            int            clrColor;
            char *         pszLegend;
            BOOL           fXDate;
            kSortedList<kWarningEntry>  *plistWarnings; /* this is really an unsorted list.*/

            void        addWarning(kError::enmErrors enmErrorCd);

        public:
            kGraphDataSet() throw(kError::enmErrors);
            kGraphDataSet(const char *pszSql, int enmTypeCd,
                          kSortedList<kWarningEntry>  *plistWarnings) throw(kError::enmErrors);
            ~kGraphDataSet(void);
            double      maxX(void) const;
            double      minX(void) const;
            double      maxY(void) const;
            double      minY(void) const;

            /* since i didn't have time to implement an unsorted list, i use a unsorted sorted list.*/
            BOOL operator==(const kGraphDataSet &entry) const { return FALSE || ! &entry; } /* "|| ..." due to unref param */
            BOOL operator!=(const kGraphDataSet &entry) const { return TRUE  ||   &entry; }
            BOOL operator< (const kGraphDataSet &entry) const { return FALSE || ! &entry; }
            BOOL operator<=(const kGraphDataSet &entry) const { return FALSE || ! &entry; }
            BOOL operator> (const kGraphDataSet &entry) const { return TRUE  ||   &entry; }
            BOOL operator>=(const kGraphDataSet &entry) const { return TRUE  ||   &entry; }

            BOOL operator==(const char *pszText) const
                { return pszLegend != NULL && pszText != NULL && stricmp(pszLegend, pszText) == 0; }
            BOOL operator!=(const char *pszText) const
                { return pszLegend != NULL && pszText != NULL && stricmp(pszLegend, pszText) != 0; }
            BOOL operator< (const char *pszText) const
                { return pszLegend != NULL && pszText != NULL && stricmp(pszLegend, pszText) < 0; }
            BOOL operator<=(const char *pszText) const
                { return pszLegend != NULL && pszText != NULL && stricmp(pszLegend, pszText) <= 0; }
            BOOL operator> (const char *pszText) const
                { return pszLegend != NULL && pszText != NULL && stricmp(pszLegend, pszText) > 0; }
            BOOL operator>=(const char *pszText) const
                { return pszLegend != NULL && pszText != NULL && stricmp(pszLegend, pszText) >= 0; }

            void        setColor(const char *pszColor);
            void        setColor(gdImagePtr pGraph);
            void        setLegend(const char *pszLegend);

            int         getColor(void) const   { return clrColor;  }
            const char *getLegend(void) const  { return pszLegend; }
            BOOL        getXDate(void) const   { return fXDate;    }

            friend long _System dbDataSetCallBack(const char *pszValue, const char *pszFieldName, void *pvUser);
    };


    /**
     * Graph.
     * @author      knut st. osmundsen
     */
    class kGraph
    {
        public:
            enum  enmType       { unknown, lines, pie };
            enum  enmSubType    { normal };

        private:
            gdImagePtr                  pGraph;
            kSortedList<kGraphDataSet>  listDataSets; /* this should really be an FIFO not a sorted list. */
            kSortedList<kWarningEntry>  listWarnings; /* this is really an unsorted list.*/

            /**@cat tag data */
            enmType    enmTypeCd;
            enmSubType enmSubTypeCd;
            char       *pszFilename;
            char       *pszTitle;
            char       *pszTitleX;
            char       *pszTitleY;
            char       *pszBackground;
            BOOL        fLegend;
            long        cX;
            long        cY;
            double      rdStartX, rdEndX; //scale x
            double      rdStartY, rdEndY; //scale y
            BOOL        fXDate;
            unsigned long ulBGColor;
            unsigned long ulFGColor;
            unsigned long ulAxisColor;

            /**@cat internal data */
            POINTL      ptlOrigo;
            POINTL      ptlYEnd;
            POINTL      ptlXEnd;
            double      rdMaxX, rdMinX;
            double      rdMaxY, rdMinY;

            /**@cat colors */
            int         clrBackground;
            int         clrForeground;
            int         clrAxis;

            /**@cat internal function */
            void    analyseTag(const kTag &tag, const char *pszBaseDir)     throw(kError::enmErrors);
            void    fetchData(const char *pszSql)   throw(kError::enmErrors);
            kGraphDataSet *findOrCreateDataSet(const char *pszLegend) throw(kError::enmErrors);

            void    createBaseGraph(void)           throw(kError::enmErrors);
            void    drawLines(void)                 throw(kError::enmErrors);
            void    drawLine(const kGraphDataSet *pDataSet) throw(kError::enmErrors);
            void    drawLegend(void)                throw(kError::enmErrors);

            void    destroy(void);
            void    addWarning(kError::enmErrors enmErrorCd);

            double  maxX(void);
            double  minX(void);
            double  maxY(void);
            double  minY(void);

            unsigned long    readColor(const char *pszColor);
            int     setColor(unsigned long ulColor);
            void    setColors(void);

        public:
            kGraph(const kTag &tag, const char *pszBaseDir) throw(kError::enmErrors);
            ~kGraph(void);

            kError::enmErrors   save(void);
            unsigned long       showWarnings(FILE *phLog, const kFileEntry *pCurFile);

            /**@cat common functions */
            static double       dateToDbl(const char *pszDate);
            static BOOL         dblToDate(double rdDate, char *pszDate);
            static BOOL         isDate(const char *pszDate);

            friend long _System dbGraphCallBack(const char *pszValue, const char *pszFieldName, void *pvUser);
    };
#endif /*_kHtmlPC_h_*/
