#define PROGRAMS 5
static Program programs[PROGRAMS] = {
  { 1535, 3,
    {
      {500, 1402, IF, 0, 0, 0.6 },
      {1401, 1533, GOTO, 0, 0, 1 },
      {1533, 0, FOR, 10, 30, 0 },
    },
    1, { 1534, }
  },
  { 1131, 1,
    {
      {1129, 0, FOR, 20, 50, 0 },
    },
    1, { 1130, }
  },
  { 1685, 2,
    {
      {1682, 1166, FOR, 10, 20, 0 },
      {1683, 0, FOR, 10, 20, 0 },
    },
    1, { 1684, }
  },
  { 1912, 0,
    {
    },
    1, { 1911, }
  },
  { 505, 4,
    {
      {500, 503, IF, 0, 0, 0.5 },
      {501, 0, GOTO, 0, 0, 1 },
      {502, 503, GOTO, 0, 0, 1 },
      {503, 0, FOR, 10, 20, 0 },
    },
    1, { 504, }
  },
};
