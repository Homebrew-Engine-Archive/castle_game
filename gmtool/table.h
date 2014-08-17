#ifndef TABLE_H_
#define TABLE_H_

#include <iosfwd>
#include <string>
#include <vector>

namespace gmtool
{
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
        Table()
            : mRowsCount(0)
            , mShowHeader(false) {}
        
        void ShowHeader(bool show);
        
        void AppendColumn(const std::string &title, Alignment alignment = Alignment::Left, unsigned indent = 3);
        void AppendRow(const std::vector<std::string> &row);

        friend std::ostream& operator<<(std::ostream &out, const Table &table);

    private:
        unsigned mRowsCount;
        bool mShowHeader;
        std::vector<Column> mColumns;

        void Print(std::ostream &out) const;
    };
}

#endif // TABLE_H_
