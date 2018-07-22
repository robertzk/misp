def substitute_dict(d, replacement):
  return dict( (k, ((d[k] in replacement) and replacement[d[k]] or d[k])) for k in d )

x = { "foo": "bar", "bum": "butt" }
y = { "butt": "poo" }

print substitute_dict(x, y)

