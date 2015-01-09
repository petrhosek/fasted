#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "fasted.h"

PyObject *
fasted_edit_distance(PyObject *self, PyObject *args)
{
    PyObject *py_list1, *py_list2;
    Py_ssize_t len1, len2;

    int *s1 = NULL, *s2 = NULL;
    int i, result = 0;

    if (!PyArg_ParseTuple(args, "O!O!", &PyList_Type, &py_list1, &PyList_Type, &py_list2))
        return NULL;

    len1 = PyList_Size(py_list1);
    s1 = calloc(len1, sizeof(int));
    if (s1 == NULL) {
        PyErr_NoMemory();
        goto out;
    }
    
    for(i = 0; i < len1; i++) {
        PyObject *py_value = PyList_GetItem(py_list1, i);
        s1[i] = PyInt_AsLong(py_value);
        if (PyErr_Occurred() != NULL) {
            PyErr_SetObject(PyExc_TypeError, py_value);
            goto out;
        }
    }

    len2 = PyList_Size(py_list2);
    s2 = calloc(len2, sizeof(int));
    if (s2 == NULL) {
        PyErr_NoMemory();
        goto out;
    }

    for(i = 0; i < len2; i++) {
        PyObject *py_value = PyList_GetItem(py_list2, i);
        s2[i] = PyInt_AsLong(py_value);
        if (PyErr_Occurred() != NULL) {
            PyErr_SetObject(PyExc_TypeError, py_value);
            goto out;
        }
    }

    result = levenshtein_distance(s1, len1, s2, len2);
    if (result == -1) {
        PyErr_NoMemory();
        goto out;
    }

out:
    free(s2);
    free(s1);
    return PyInt_FromLong((long)result);
}


PyMethodDef module_methods[] = {
    {"edit_distance", fasted_edit_distance, METH_VARARGS,
     "edit_distance(list1, list2)\n\n"
     "Compute the edit distance between list1 and list2."},
    {NULL}
};

PyObject*
moduleinit(PyObject* m)
{
    if (m == NULL)
        return NULL;

    return m;
}

#if PY_MAJOR_VERSION < 3
  PyMODINIT_FUNC
  init_fasted(void)
  {
      PyObject* m;
      m = Py_InitModule3("_fasted", module_methods,
                         "Fast edit distance.");
      moduleinit(m);
  }
#else
  struct PyModuleDef moduledef = {
      PyModuleDef_HEAD_INIT,
      "_fasted",                      /* m_name */
      "Fast edit distance.",          /* m_doc */
      -1,                              /* m_size */
      module_methods,                  /* m_methods */
      NULL,                            /* m_reload */
      NULL,                            /* m_traverse */
      NULL,                            /* m_clear */
      NULL,                            /* m_free */
  };

  PyMODINIT_FUNC
  PyInit__fasted(void)
  {
      PyObject* m;
      m = PyModule_Create(&moduledef);
      return moduleinit(m);
  }
#endif
