#include "table.h"

#include <iostream>

namespace gmtool
{
    void PrintCell(std::ostream &out, const std::string &text, Alignment alignment, unsigned width, unsigned indent)
    {
        out.width(indent);
        out << out.fill();

        out.width(width);
        
        if(alignment == Alignment::Left) {
            out << std::left;
        } else if(alignment == Alignment::Right) {
            out << std::right;
        }
        out << text;
    }
    
    void Column::Append(const std::string &cell)
    {
        mValues.push_back(cell);
        mWidth = std::max<unsigned>(mWidth, cell.length());
    }

    void Column::PrintHeader(std::ostream &out) const
    {
        PrintCell(out, mValues.front(), mAlign, mWidth, mIndent);
    }
    
    void Column::Print(std::ostream &out, unsigned row) const
    {
        PrintCell(out, mValues[row + 1], mAlign, mWidth, mIndent);
    }

    void Table::ShowHeader(bool show)
    {
        mShowHeader = show;
    }
    
    void Table::AppendColumn(const std::string &title, Alignment alignment, unsigned indent)
    {
        mColumns.emplace_back(title, alignment, indent);
    }
    
    void Table::AppendRow(const std::vector<std::string> &cells)
    {
        for(unsigned i = 0; i < std::min(cells.size(), mColumns.size()); ++i) {
            mColumns[i].Append(cells[i]);
        }
        ++mRowsCount;
    }

    void Table::Print(std::ostream &out) const
    {
        if(mShowHeader) {
            for(const Column &column : mColumns) {
                column.PrintHeader(out);
            }
            out << std::endl;
        }

        for(unsigned i = 0; i < mRowsCount; ++i) {
            for(const Column &column : mColumns) {
                column.Print(out, i);
            }
            out << std::endl;
        }
    }        
    
    std::ostream& operator<<(std::ostream &out, const Table &table)
    {
        table.Print(out);
        return out;
    }
}
