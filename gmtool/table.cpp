#include "table.h"
#include <iostream>

namespace gmtool
{
    void Column::Extend(unsigned width)
    {
        mWidth = std::max<unsigned>(width, mWidth);
    }

    void Column::PrintHeader(std::ostream &out) const
    {
        PrintCell(out, mTitle);
    }
    
    void Column::PrintCell(std::ostream &out, const std::string &text) const
    {
        out.width(mIndent);
        out << out.fill();

        out.width(mWidth);
        
        if(mAlign == Alignment::Left) {
            out << std::left;
        } else if(mAlign == Alignment::Right) {
            out << std::right;
        }
        out << text;
    }
    
    void Table::AppendColumn(const std::string &title, Alignment alignment, unsigned indent)
    {
        mColumns.emplace_back(title, alignment, indent);
    }

    template<class FwdIter1, class FwdIter2, class BinaryOperation>
    void ZipWith(FwdIter1 first1, FwdIter1 last1, FwdIter2 first2, FwdIter2 last2, BinaryOperation op)
    {
        while((first1 != last1) && (first2 != last2)) {
            op(*first1, *first2);
            ++first1;
            ++first2;
        }
    }
    
    void Table::AppendRow(const std::vector<std::string> &row)
    {
        mCells.push_back(row);
        ZipWith(mColumns.begin(), mColumns.end(), row.begin(), row.end(),
                [](Column &column, const std::string &cell) {
                    column.Extend(cell.length());
                });
    }

    void Table::PrintHeader(std::ostream &out) const
    {
        for(const Column &column : mColumns) {
            column.PrintHeader(out);
        }
        out << std::endl;
    }
    
    void Table::Print(std::ostream &out) const
    {
        for(const std::vector<std::string> &row : mCells) {
            ZipWith(mColumns.begin(), mColumns.end(), row.begin(), row.end(),
                    [&out](const Column &column, const std::string &cell) {
                        column.PrintCell(out, cell);
                    });
            out << std::endl;
        }
    }
}
