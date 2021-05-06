#ifndef SATABLE_H
#define SATABLE_H
//Qt
#include <QVector>
#include <QHash>
#include <QDebug>
//std
#include <memory>
//SA
#include "SAAlgorithm.h"

/**
 * @brief 系列数据,系列允许设置名字
 */
template<typename T>
class SAVector : public QVector<T>
{
public:
    SAVector() : QVector<T>()
    {
    }


    SAVector(int size) : QVector<T>(size)
    {
    }


    SAVector(int size, const T& t) : QVector<T>(size, t)
    {
    }


    SAVector(std::initializer_list<T> args) : QVector<T>(args)
    {
    }


    SAVector(const QString& n) : QVector<T>(), mName(n)
    {
    }


    const QString& name() const;
    QString& name();
    void setName(const QString& n);
    QString getName() const;

private:
    QString mName;///< 系列名，参考pandas.Series.name
};

template<typename T>
const QString& SAVector<T>::name() const
{
    return (mName);
}


template<typename T>
QString& SAVector<T>::name()
{
    return (mName);
}


template<typename T>
void SAVector<T>::setName(const QString& n)
{
    mName = n;
}


template<typename T>
QString SAVector<T>::getName() const
{
    return (mName);
}


/**
 * @brief 以行为基础的数据表
 */
template<typename T>
class SARowTable
{
public:
    enum Mode {
        FixedMode,      ///< 固定模式，表的列不会随着行的变化而变化(默认)
        ExpandMode      ///< 扩展模式，表的列是浮动的，如果插入一行比原来要宽，会自动扩充表的列数
    };
    typedef T					Type;
    typedef SAVector<T>				SeriesType;
    typedef std::shared_ptr<SeriesType>		SeriesPtr;
    typedef typename SAVector<T>::iterator		SeriesIterator;
    typedef typename SAVector<T>::const_iterator	SeriesConstIterator;
    typedef SARowTable<T>				TableType;
    typedef std::shared_ptr<TableType>		TablePtr;
    static SeriesPtr makeSeries();
    static SeriesPtr makeSeries(const QString& n);
    static SeriesPtr makeSeries(std::initializer_list<T> args);
    static TablePtr makeTable();

    SARowTable();
    SARowTable(int rows, int columns);
    void resize(int r, int c);

    /**
     * @brief 填充元素
     * @param v
     */
    void fill(const T& v);
    const T& at(int r, int c) const;
    T& at(int r, int c);

    T cell(int r, int c) const;
    void appendRow(SeriesPtr row);
    void appendRow(std::initializer_list<T> args, const QString& n);

    template<typename Ite1, typename Ite2>
    void appendColumn(Ite1 b, Ite2 e);
    void appendColumn(std::initializer_list<T> args);
    int nameToIndex(const QString& n, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;
    SeriesPtr& row(int r);
    const SeriesPtr& row(int r) const;
    SeriesPtr& row(const QString& n);
    const SeriesPtr& row(const QString& n) const;

    //返回一列数据，返回一个SeriesPtr，这个seriesPtr的写操作不会影响table
    SeriesPtr colunm(int c) const;

    SeriesType& operator[](int r);
    const SeriesType& operator[](int r) const;

    void reserve(int size);

    /**
     * @brief 以最大列数进行列数修正，保证所有行同列
     */
    void fixSize();

    ///
    /// \brief 表的行数
    /// \return
    ///
    int rowCount() const;

    ///
    /// \brief 表的列数
    /// \return
    ///
    int columnCount() const;

    /**
     * @brief 设置表格的模式
     * @param m 模式 @ref Mode
     */
    void setMode(Mode m);

    /**
     * @brief 获取模式
     * @return
     */
    Mode getMode() const;

    /**
     * @brief 清空
     */
    void clear();

    void setName(const QString& n);
    QString getName() const;

    /**
     * @brief rowNames
     * @return
     */
    QStringList rowNames() const;

    /**
     * @brief 设置行名，如果是个空的表会生成一个默认行
     * @param ns
     */
    void setRowNames(const QStringList& ns);

    /**
     * @brief 提取某个值等于value作为新表
     * @param field
     * @param value
     * @param cs
     * @return
     */
    TablePtr takeByValue(const QString& field, T value, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

    /**
     * @brief groupby
     * @param field
     * @param cs
     * @return
     */
    QPair<QList<TablePtr>, QList<T> > groupBy(const QString& field, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

    /**
     * @brief orderBy
     * @param sn
     */
    void orderBy(const QString& sn, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
    void orderBy(int rindex);

    /**
     * @brief 查找第一个大于或等于某个元素的位置
     * @note 需要确保已经进行过排序
     * @param v
     * @return
     */
    QPair<T, int> lowerBound(const T& v, const QString& sortedfield, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;
    QPair<T, int> lowerBound(const T& v, int r) const;
    QPair<T, int> upperBound(const T& v, const QString& sortedfield, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;
    QPair<T, int> upperBound(const T& v, int r) const;


private:
    SAVector<SeriesPtr> m_d;
    size_t m_columns;
    Mode m_mode;
    SeriesPtr m_nullseries;
};

namespace SA {
template<typename T>
typename SARowTable<T>::TablePtr takeByValue(const SARowTable<T>& table, const QString& field, T value, Qt::CaseSensitivity cs = Qt::CaseInsensitive)
{
    const int r = table.nameToIndex(field, cs);

    typename SARowTable<T>::TablePtr res = SARowTable<T>::makeTable();

    res->setName(table.getName());
    res->setRowNames(table.rowNames());
    int csize = table.columnCount();

    for (int i = 0; i < csize; ++i)
    {
        if (table.cell(r, i) == value) {
            typename SARowTable<T>::SeriesPtr col = table.colunm(i);
            res->appendColumn(col->begin(), col->end());
        }
    }
    return (res);
}


template<typename T>
typename SARowTable<T>::TablePtr takeByValue(const SARowTable<T>& table, int r, T value)
{
    typename SARowTable<T>::TablePtr res = SARowTable<T>::makeTable();

    res->setName(table.getName());
    res->setRowNames(table.rowNames());
    int csize = table.columnCount();

    for (int i = 0; i < csize; ++i)
    {
        if (table.cell(r, i) == value) {
            typename SARowTable<T>::SeriesPtr col = table.colunm(i);
            res->appendColumn(col->begin(), col->end());
        }
    }
    return (res);
}


/**
 * @brief groupby 对某个字段执行group by操作
 * @param table
 * @param field
 * @return 返回一个pair，first：group by后的结构表，second，group by的结果
 */
template<typename T>
QPair<QList<typename SARowTable<T>::TablePtr>, QList<T> > groupby(const SARowTable<T>& table, const QString& field, Qt::CaseSensitivity cs = Qt::CaseInsensitive)
{
    QList<typename SARowTable<T>::TablePtr> restables;
    QList<T> gr;

    int rindex = table.nameToIndex(field, cs);

    Q_ASSERT_X(rindex >= 0, "groupby", "unknow field");
    typename SARowTable<T>::SeriesPtr r = table.row(rindex);
    if (r == nullptr) {
        return (qMakePair(restables, gr));
    }
    gr = r->toList().toSet().toList();
    std::sort(gr.begin(), gr.end());
    for (T v : gr)
    {
        restables.append(takeByValue(table, rindex, v));
    }
    return (qMakePair(restables, gr));
}


template<typename T>
class ValueWithIndex {
public:
    ValueWithIndex() : index(-1)
    {
    }


    ValueWithIndex(const T& v, int i) : value(v), index(i)
    {
    }


    T value;
    int index;
};

template<typename T>
bool operator <(const ValueWithIndex<T>& a, const ValueWithIndex<T>& b)
{
    return (a.value < b.value);
}


/**
 * @brief 把序列转换为带序号的序列
 * @param p
 * @return 用于带序号的排序用
 */
template<typename T>
std::shared_ptr<SAVector<ValueWithIndex<T> > > makeIndexSeries(typename SARowTable<T>::SeriesPtr p)
{
    std::shared_ptr<SAVector<ValueWithIndex<T> > > res = std::make_shared<SAVector<ValueWithIndex<T> > >();

    res->setName(p->getName());
    const int s = p->size();

    res->reserve(s);
    for (int i = 0; i < s; ++i)
    {
        res->push_back(ValueWithIndex<T>(p->at(i), i));
    }
    return (res);
}


template<typename T>
void orderBy(SARowTable<T>& table, const QString& field, Qt::CaseSensitivity cs = Qt::CaseInsensitive)
{
    const int r = table.nameToIndex(field, cs);

    typename SARowTable<T>::SeriesPtr row = table.row(r);
    Q_ASSERT_X(row != nullptr, "orderBy", "unknow field");

    auto ordser = makeIndexSeries<T>(row);

    std::sort(ordser->begin(), ordser->end());
    int rowcount = table.rowCount();

    //开始逐一转换
    for (int rc = 0; rc < rowcount; ++rc)
    {
        typename SARowTable<T>::SeriesPtr series = table.row(rc);
        typename SARowTable<T>::SeriesPtr ns = SARowTable<T>::makeSeries(series->getName());
        ns->reserve(series->size());
        for (auto i = ordser->begin(); i != ordser->end(); ++i)
        {
            ns->push_back(series->at((*i).index));
        }
        series.swap(ns);
    }
}


template<typename T>
void orderBy(SARowTable<T>& table, int r)
{
    typename SARowTable<T>::SeriesPtr row = table.row(r);
    Q_ASSERT_X(row != nullptr, "orderBy", "unknow field");
    auto ordser = makeIndexSeries<T>(row);

    std::sort(ordser->begin(), ordser->end());
    int rowcount = table.rowCount();

    //开始逐一转换
    for (int rc = 0; rc < rowcount; ++rc)
    {
        typename SARowTable<T>::SeriesPtr series = table.row(rc);
        typename SARowTable<T>::SeriesPtr ns = SARowTable<T>::makeSeries(series->getName());
        ns->reserve(series->size());
        for (auto i = ordser->begin(); i != ordser->end(); ++i)
        {
            ns->push_back(series->at((*i).index));
        }
        series.swap(ns);
    }
}
}


template<typename T>
SARowTable<T>::SARowTable() : m_columns(0), m_mode(FixedMode)
{
}


template<typename T>
SARowTable<T>::SARowTable(int rows, int columns) : m_mode(FixedMode)
{
    m_d.clear();
    m_d.reserve(rows);
    for (int i = 0; i < rows; ++i)
    {
        m_d.push_back(SeriesType(columns));
    }
    m_columns = columns;
}


/**
 * @brief 改变table 的大小
 * @param r
 * @param c
 */
template<typename T>
void SARowTable<T>::resize(int r, int c)
{
    m_d.resize(r);
    for (SeriesPtr& row : m_d)
    {
        if (row == nullptr) {
            row = makeSeries();
        }
        row->resize(c);
    }
    m_columns = c;
}


template<typename T>
void SARowTable<T>::fill(const T& v)
{
    for (SeriesPtr r : m_d)
    {
        r->fill(v);
    }
}


template<typename T>
const T& SARowTable<T>::at(int r, int c) const
{
    return (m_d.at(r)->at(c));
}


template<typename T>
T& SARowTable<T>::at(int r, int c)
{
    return (m_d[r]->operator [](c));
}


/**
 * @brief 获取单元格
 * @param r
 * @param c
 * @return 如果没有或超范围，返回默认构造
 */
template<typename T>
T SARowTable<T>::cell(int r, int c) const
{
    if (r < m_d.size()) {
        const SeriesPtr& rr = row(r);
        if (c < rr->size()) {
            return (rr->at(c));
        }
    }
    return (T());
}


template<typename T>
void SARowTable<T>::appendRow(SeriesPtr row)
{
    size_t s = row->size();

    if ((s == m_columns) || (0 == m_columns)) {
        m_d.push_back(row);
    }else if (s < m_columns) {//在结尾补充
        row->resize(m_columns);
        m_d.push_back(row);
    }else {//s>m_columns
        if (getMode() == ExpandMode) {
            m_d.push_back(row);
            fixSize();
        }else{
            //固定模式的插入
            row->resize(m_columns);
            m_d.push_back(row);
        }
    }
}


template<typename T>
void SARowTable<T>::appendRow(std::initializer_list<T> args, const QString& n)
{
    SeriesPtr r = makeSeries(args);

    r->setName(n);
    appendRow(r);
}


template<typename T>
template<typename Ite1, typename Ite2>
void SARowTable<T>::appendColumn(Ite1 b, Ite2 e)
{
    const int rc = rowCount();
    auto it = b;

    for (int i = 0; i < rc; ++i)
    {
        it = b + i;
        if (it < e) {
            row(i)->push_back(*it);
        }else{
            row(i)->push_back(T());
        }
    }
    ++m_columns;
}


template<typename T>
void SARowTable<T>::appendColumn(std::initializer_list<T> args)
{
    const int rc = rowCount();

    for (int i = 0; i < rc; ++i)
    {
        if (i < args.size()) {
            row(i)->push_back(args[i]);
        }else{
            row(i)->push_back(T());
        }
    }
    ++m_columns;
}


template<typename T>
typename SARowTable<T>::SeriesPtr SARowTable<T>::makeSeries(const QString& n)
{
    return (std::make_shared<SeriesType>(n));
}


template<typename T>
typename SARowTable<T>::SeriesPtr SARowTable<T>::makeSeries(std::initializer_list<T> args)
{
    return (std::make_shared<SeriesType>(args));
}


template<typename T>
typename SARowTable<T>::TablePtr SARowTable<T>::makeTable()
{
    return (std::make_shared<typename SARowTable<T>::TableType>());
}


template<typename T>
typename SARowTable<T>::SeriesPtr SARowTable<T>::makeSeries()
{
    return (std::make_shared<SeriesType>());
}


template<typename T>
int SARowTable<T>::nameToIndex(const QString& n, Qt::CaseSensitivity cs) const
{
    int r = rowCount();

    for (int i = 0; i < r; ++i)
    {
        if (row(i)->name().compare(n, cs) == 0) {
            return (i);
        }
    }
    return (-1);
}


/**
 * @brief 获取行引用
 * @param r
 * @return
 */
template<typename T>
typename SARowTable<T>::SeriesPtr& SARowTable<T>::row(int r)
{
    return (m_d[r]);
}


/**
 * @brief 获取行引用
 * @param r
 * @return
 */
template<typename T>
const typename SARowTable<T>::SeriesPtr& SARowTable<T>::row(int r) const
{
    return (m_d[r]);
}


template<typename T>
typename SARowTable<T>::SeriesPtr& SARowTable<T>::row(const QString& n)
{
    int r = nameToIndex(n);

    if ((r < 0) || (r > rowCount())) {
        return (m_nullseries);
    }
    return (row(r));
}


template<typename T>
const typename SARowTable<T>::SeriesPtr& SARowTable<T>::row(const QString& n) const
{
    int r = nameToIndex(n);

    if ((r < 0) || (r > rowCount())) {
        return (m_nullseries);
    }
    return (row(r));
}


template<typename T>
typename SARowTable<T>::SeriesPtr SARowTable<T>::colunm(int c) const
{
    int rsize = rowCount();
    SeriesPtr col = std::make_shared<SeriesType>(rsize);

    for (int r = 0; r < rsize; ++r)
    {
        col->operator [](r) = cell(r, c);
    }
    return (col);
}


template<typename T>
typename SARowTable<T>::SeriesType& SARowTable<T>::operator[](int r)
{
    return (*(row(r)));
}


template<typename T>
const typename SARowTable<T>::SeriesType& SARowTable<T>::operator[](int r) const
{
    return (*(row(r)));
}


template<typename T>
void SARowTable<T>::reserve(int size)
{
    for (SeriesPtr p : m_d)
    {
        p->reserve(size);
    }
}


template<typename T>
void SARowTable<T>::fixSize()
{
    std::vector<int> ss;

    for (const SeriesPtr& r : m_d)
    {
        ss.push_back(r->size());
    }
    int maxsize = *(std::max_element(ss.begin(), ss.end()));

    for (SeriesPtr& r : m_d)
    {
        if (r->size() < maxsize) {
            r->resize(maxsize);
        }
    }
    m_columns = maxsize;
}


template<typename T>
int SARowTable<T>::rowCount() const
{
    return (m_d.size());
}


template<typename T>
int SARowTable<T>::columnCount() const
{
    return (m_columns);
}


template<typename T>
void SARowTable<T>::setMode(SARowTable::Mode m)
{
    m_mode = m;
}


template<typename T>
typename SARowTable<T>::Mode SARowTable<T>::getMode() const
{
    return (m_mode);
}


template<typename T>
void SARowTable<T>::clear()
{
    m_d.clear();
    m_columns = 0;
}


template<typename T>
void SARowTable<T>::setName(const QString& n)
{
    m_d.setName(n);
}


template<typename T>
QString SARowTable<T>::getName() const
{
    return (m_d.getName());
}


template<typename T>
QStringList SARowTable<T>::rowNames() const
{
    QStringList r;

    for (SeriesPtr p : m_d)
    {
        r.append(p->getName());
    }
    return (r);
}


template<typename T>
void SARowTable<T>::setRowNames(const QStringList& ns)
{
    int s = ns.size();

    for (int i = 0; i < s; ++i)
    {
        if (i < rowCount()) {
            row(i)->setName(ns[i]);
        }else{
            SeriesPtr r = makeSeries(ns[i]);
            appendRow(r);
        }
    }
}


template<typename T>
typename SARowTable<T>::TablePtr SARowTable<T>::takeByValue(const QString& field, T value, Qt::CaseSensitivity cs) const
{
    return (SA::takeByValue(*this, field, value, cs));
}


template<typename T>
QPair<QList<typename SARowTable<T>::TablePtr>, QList<T> > SARowTable<T>::groupBy(const QString& field, Qt::CaseSensitivity cs) const
{
    return (SA::groupby(*this, field, cs));
}


template<typename T>
void SARowTable<T>::orderBy(const QString& sn, Qt::CaseSensitivity cs)
{
    SA::orderBy(*this, sn, cs);
}


template<typename T>
void SARowTable<T>::orderBy(int rindex)
{
    SA::orderBy(*this, rindex);
}


template<typename T>
QPair<T, int> SARowTable<T>::lowerBound(const T& v, const QString& sortedfield, Qt::CaseSensitivity cs) const
{
    const int r = nameToIndex(sortedfield, cs);

    return (lowerBound(v, r));
}


template<typename T>
QPair<T, int> SARowTable<T>::lowerBound(const T& v, int r) const
{
    typename SARowTable<T>::SeriesPtr prow = row(r);
    SeriesIterator ite = std::lower_bound(prow->begin(), prow->end(), v);

    if (ite == prow->end()) {
        return (qMakePair<T, int>(prow->back(), prow->size()-1));
    }
    size_t dis = std::distance(prow->begin(), ite);

    return (qMakePair<T, int>(*ite, dis));
}


template<typename T>
QPair<T, int> SARowTable<T>::upperBound(const T& v, const QString& sortedfield, Qt::CaseSensitivity cs) const
{
    const int r = nameToIndex(sortedfield, cs);

    return (upperBound(v, r));
}


template<typename T>
QPair<T, int> SARowTable<T>::upperBound(const T& v, int r) const
{
    typename SARowTable<T>::SeriesPtr prow = row(r);
    SeriesIterator ite = std::upper_bound(prow->begin(), prow->end(), v);

    if (ite == prow->end()) {
        return (qMakePair<T, int>(prow->back(), prow->size()-1));
    }
    size_t dis = std::distance(prow->begin(), ite);

    return (qMakePair<T, int>(*ite, dis));
}


template<typename T>
QDebug operator<<(QDebug debug, const SARowTable<T>& t)
{
    QDebugStateSaver saver(debug);
    int rs = t.rowCount();
    QStringList rns = t.rowNames();
    int maxlen = 0;

    for (const QString& r : rns)
    {
        maxlen = qMax(maxlen, r.size());
    }

    for (int i = 0; i < rs; ++i)
    {
        typename SARowTable<T>::SeriesPtr r = t.row(i);
        QString name = r->getName();
        if (name.size() < maxlen) {
            name.resize(maxlen, ' ');
        }
        debug.noquote() << name << ":";
        int cs = r->size();
        if (cs > 10) {
            for (int j = 0; j < 5; ++j)
            {
                debug.noquote() << r->at(j) << ",";
            }
            debug.noquote() << "  ......  ";
            for (int j = cs-6; j < cs; ++j)
            {
                debug.noquote() << r->at(j) << ",";
            }
        }else{
            for (int j = 0; j < cs; ++j)
            {
                debug << r->at(j) << ",";
            }
        }
        debug << "\n";
    }

    return (debug);
}


#endif // SATABLE_H
