//RUN INSTRUCTIONS
//g++ -I /usr/include/python3.7 main.cpp -lpython3.7m
//./a.out multiply multiply 2 5

#include <python3.7/Python.h>
#include <iostream>

static int numargs=0;

/* Return the number of arguments of the application command line */
static PyObject*
emb_numargs(PyObject *self, PyObject *args)
{
    if(!PyArg_ParseTuple(args, ":numargs"))
        return NULL;
    return Py_BuildValue("i", numargs);
}

// list its name and address in a “method table”:
static PyMethodDef EmbMethods[] = {
    {"numargs", emb_numargs, METH_VARARGS,
     "Return the number of arguments received by the process."},
    {NULL, NULL, 0, NULL}
};

// The method table must be referenced in the module definition structure:
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,  /* m_base */
    "spam",                 /* m_name */
    NULL,                   /* m_doc */
    -1,                     /* m_size */
    EmbMethods            /* m_methods */
};
static PyObject*
PyInit_emb(void)
{
    return PyModule_Create(&moduledef);
}

int main(int argc, char* argv[])
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;

    if (argc < 3) {
        fprintf(stderr,"Usage: call pythonfile funcname [args]\n");
        return 1;
    }

    PyImport_AppendInittab("emb", &PyInit_emb);
    Py_Initialize();

    numargs = argc;

    PyModule_Create(&moduledef);

    //Py_InitModule("emb", EmbMethods);
    PyImport_AppendInittab("emb", &PyInit_emb);

    pName = PyUnicode_FromString(argv[1]);
    /* Error checking of pName left out */
    
    // Convert char** to wchar_t**
    wchar_t* wideArgs[argc];
    for(int i = 0; i < argc; i++){
        wchar_t* newArg;
        size_t newsize = strlen(argv[i]) + 1;
        newArg = new wchar_t[newsize];
        mbstowcs(newArg, argv[i], newsize);
        wideArgs[i] = newArg; 
    }
    
    //fixes no module named.. error
    PySys_SetArgv(argc, wideArgs);

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, argv[2]);
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(argc - 3);
            for (int i = 0; i < argc - 3; ++i) {
                pValue = PyLong_FromLong(atoi(argv[i + 3]));
                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return 1;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 1;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
        return 1;
    }
    Py_Finalize();
    return 0;
}
