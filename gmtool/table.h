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
            : mAlign(alignment)
            , mWidth(0)
            , mIndent(indent)
            , mValues() {
            Append(title);
        }

        void Append(const std::string &cell);
        void PrintHeader(std::ostream &out) const;
        void Print(std::ostream &out, unsigned row) const;
        
    private:
        Alignment mAlign;
        unsigned mWidth;
        unsigned mIndent;
        std::vector<std::string> mValues;
    };

    class Table
    {
    public:
        Table(const std::vector<Column> &columns = std::vector<Column>())
            : mRowsCount(0)
            , mColumns(columns) {}

        void AppendRow(const std::vector<std::string> &row);
        void AppendColumn(const std::string &text, Alignment alignment = Alignment::Left, unsigned indent = DefaultTableIndent);
        void PrintHeader(std::ostream &out) const;
        void Print(std::ostream &out) const;
        
    private:
        unsigned mRowsCount;
        std::vector<Column> mColumns;
    };
}

#endif // TABLE_H_
