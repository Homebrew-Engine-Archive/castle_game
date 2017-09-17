#ifndef TABLE_H_
#define TABLE_H_

#include <iosfwd>
#include <string>
#include <vector>

namespace gmtool
{
    const unsigned DefaultTableIndent = 3;
    
    enum class Alignment {Left, Right};
    
    class Column
    {
    public:
        Column(const std::string &title, Alignment alignment, unsigned indent)
            : mTitle(title)
            , mAlign(alignment)
            , mWidth(title.length())
            , mIndent(indent)
            { }

        void Extend(unsigned width);
        void PrintHeader(std::ostream &out) const;
        void PrintCell(std::ostream &out, const std::string &text) const;
        
    private:
        const std::string mTitle;
        Alignment mAlign;
        unsigned mWidth;
        unsigned mIndent;
    };

    class Table
    {
    public:
        Table(const std::vector<Column> &columns = std::vector<Column>())
            : mColumns(columns) {}

        void AppendRow(const std::vector<std::string> &row);
        void AppendColumn(const std::string &text, Alignment alignment = Alignment::Left, unsigned indent = DefaultTableIndent);
        void PrintHeader(std::ostream &out) const;
        void Print(std::ostream &out) const;
        
    private:
        std::vector<std::vector<std::string>> mCells;
        std::vector<Column> mColumns;
    };
}

#endif // TABLE_H_
