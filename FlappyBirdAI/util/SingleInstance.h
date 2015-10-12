// the macro that make a class a single instance


// put DEFINE_SINGLEINSTANCE at the end of class definition
#define DEFINE_SINGLEINSTANCE(__TYPE__) \
public: \
    static __TYPE__* GetInstance() \
    { \
        if(NULL == m_pInstance) \
            m_pInstance = new __TYPE__(); \
        return static_cast<__TYPE__*>(m_pInstance); \
    } \
     \
    ~__TYPE__(); \
     \
    static void Release() \
    { \
        if(NULL != m_pInstance) \
        { \
            delete m_pInstance; \
            m_pInstance = NULL; \
        } \
    } \
     \
protected: \
    __TYPE__(); \
    static __TYPE__ * m_pInstance; \


// put INIT_SINGLEINSTANCE at the cpp file
#define INIT_SINGLEINSTANCE(__TYPE__) \
    __TYPE__* __TYPE__::m_pInstance = NULL;