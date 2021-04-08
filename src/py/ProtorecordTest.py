import unittest
from protorecord.Reader import Reader

class TestCppInteroperability(unittest.TestCase):

	def test(self):
		reader = Reader("recording")
		self.assertEqual(reader.reason(),'')

		reader.close()

if __name__ == '__main__':
	unittest.main()