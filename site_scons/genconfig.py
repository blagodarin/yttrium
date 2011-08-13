from __future__ import with_statement

class GenConfig(object):

	def __init__(self, guardname):
		self._buffer = '#ifndef %s\n#define %s\n\n' % (guardname, guardname)

	def save(self, filename):
		self._buffer += '\n#endif\n'
		with open(filename, 'wb') as config:
			config.write(self._buffer)

	def define(self, symbol, value):
		if symbol is not None:
			self._buffer += '#define %s %s\n' % (symbol, value)
		else:
			self._buffer += '#define %s\n' % symbol

	def define_if(self, symbol, cond, value = 1):
		if cond:
			self.define(symbol, value)

	def define_select(self, symbol, cond, if_true, if_false):
		if cond:
			self.define(symbol, if_true)
		else:
			self.define(symbol, if_false)

	def new_line(self):
		self._buffer += '\n'
