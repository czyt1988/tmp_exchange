#ifndef SATABLE_H
#define SATABLE_H
//Qt
#include <QVector>
#include <QHash>
#include <QDebug>
//std
#include <memory>

/**
 * @brief 系列数据
 */
template<typename T>
class SASeries : public QVector<T>
{
public:
    SASeries() : QVector<T>()
    {
    }


    SASeries(int size) : QVector<T>(size)
    {
    }


    SASeries(int size, const T& t) : QVector<T>(size, t)
    {
    }


    SASeries(std::initializer_list<T> args) : QVector<T>(args)
    {
    }


    SASeries(const QString& n) : QVector<T>(), mName(n)
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
const QString& SASeries<T>::name() const
{
    return (mName);
}


template<typename T>
QString& SASeries<T>::name()
{
    return (mName);
}


template<typename T>
void SASeries<T>::setName(const QString& n)
{
    mName = n;
}


template<typename T>
QString SASeries<T>::getName() const
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
    typedef SASeries<T>			RowType;
    typedef std::shared_ptr<RowType>	RowPtr;
    typedef SASeries<RowPtr>		TableType;

    SARowTable();
    SARowTable(int rows, int columns);
    void resize(int r, int c);
    const T& at(int r, int c) const;
    T& at(int r, int c);
    T cell(int r, int c) const;
    void appendRow(RowPtr row);
    void appendRow(std::initializer_list<T> args, const QString& n);

    template<typename Ite1, typename Ite2>
    void appendColumn(Ite1 b, Ite2 e);
    void appendColumn(std::initializer_list<T> args);
    static RowPtr makeRow();
    static RowPtr makeRow(const QString& n);
    static RowPtr makeRow(std::initializer_list<T> args);
    int nameToIndex(const QString& n) const;
    RowPtr& row(int r);
    const RowPtr& row(int r) const;
    RowPtr& row(const QString& n);
    const RowPtr& row(const QString& n) const;

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

private:
    TableType m_d;
    size_t m_columns;
    Mode m_mode;
};


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
        m_d.push_back(RowType(columns));
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
    for (RowType& row : m_d)
    {
        row.resize(c);
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
        const RowPtr& rr = row(r);
        if (c < rr->size()) {
            return (rr->at(c));
        }
    }
    return (T());
}


template<typename T>
void SARowTable<T>::appendRow(RowPtr row)
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
    RowPtr r = makeRow(args);

    r->setName(n);
    appendRow(r);
}


template<typename T>
template<typename Ite1, typename Ite2>
void SARowTable<T>::appendColumn(Ite1 b, Ite2 e)
{
    auto s = std::distance(b, e);

    if (s <= rowCount()) {
        int i = 0;
        while (b != e)
        {
            row(i)->push_back(*b);
            ++b;
            ++i;
        }
    }
}


template<typename T>
void SARowTable<T>::appendColumn(std::initializer_list<T> args)
{
    for (int i = 0; i < args.size(); ++i)
    {
        row(i)->append(args[i]);
    }
}


template<typename T>
typename SARowTable<T>::RowPtr SARowTable<T>::makeRow(const QString& n)
{
    return (std::make_shared<RowType>(n));
}


template<typename T>
typename SARowTable<T>::RowPtr SARowTable<T>::makeRow(std::initializer_list<T> args)
{
    return (std::make_shared<RowType>(args));
}


template<typename T>
typename SARowTable<T>::RowPtr SARowTable<T>::makeRow()
{
    return (std::make_shared<RowType>());
}


template<typename T>
int SARowTable<T>::nameToIndex(const QString& n) const
{
    int r = rowCount();

    for (int i = 0; i < r; ++i)
    {
        if (row(i)->name() == n) {
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
typename SARowTable<T>::RowPtr& SARowTable<T>::row(int r)
{
    return (m_d[r]);
}


/**
 * @brief 获取行引用
 * @param r
 * @return
 */
template<typename T>
const typename SARowTable<T>::RowPtr& SARowTable<T>::row(int r) const
{
    return (m_d[r]);
}


template<typename T>
typename SARowTable<T>::RowPtr& SARowTable<T>::row(const QString& n)
{
    int r = nameToIndex(n);

    if ((r < 0) || (r > rowCount())) {
        return (nullptr);
    }
    return (row(r));
}


template<typename T>
const typename SARowTable<T>::RowPtr& SARowTable<T>::row(const QString& n) const
{
    int r = nameToIndex(n);

    if ((r < 0) || (r > rowCount())) {
        return (nullptr);
    }
    return (row(r));
}


template<typename T>
void SARowTable<T>::fixSize()
{
    std::vector<int> ss;

    for (const RowPtr& r : m_d)
    {
        ss.push_back(r->size());
    }
    int maxsize = *(std::max_element(ss.begin(), ss.end()));

    for (RowPtr& r : m_d)
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
    m_d->setName(n);
}


template<typename T>
QString SARowTable<T>::getName() const
{
    return (m_d->getName());
}


template<typename T>
QStringList SARowTable<T>::rowNames() const
{
    QStringList r;

    for (RowPtr p : m_d)
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
            RowPtr r = makeRow(ns[i]);
            appendRow(r);
        }
    }
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
        typename SARowTable<T>::RowPtr r = t.row(i);
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
