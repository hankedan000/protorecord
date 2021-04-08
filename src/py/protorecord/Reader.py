import os
import ProtorecordIndex_pb2
import ProtorecordTypes_pb2

# size in bytes of the Version message
PROTORECORD_VERSION_SIZE = 24

# size in bytes of the IndexSummary message
PROTORECORD_INDEX_SUMMARY_SIZE = 28

# size in bytes of the IndexItem message
PROTORECORD_INDEX_ITEM_SIZE_TIMESTAMP = 20

# size in bytes of the IndexItem message with no timestamp
PROTORECORD_INDEX_ITEM_SIZE_NO_TIMESTAMP = 12


# set if the other bits in the words are valid
PROTORECORD_FLAGS_VALID = 0x1

# set if the record experienced an error at some point
# during the recording.
PROTORECORD_FLAGS_RECORD_WRITE_ERROR = 0x2

# set if the record contains binary message data that was assumed
# to be serialized externally to library. This flag is set the first
# time Writer::write_assumed() method is called.
PROTORECORD_FLAGS_HAS_ASSUMED_DATA = 0x4

# indicating the record contains timestamped items
PROTORECORD_FLAGS_HAS_TIMESTAMPS = 0x8

class Reader():
	def __init__(self,filepath):
		# set to true if the writer was initialized succesfully
		self.initialized_ = False

		# the parsed library version from the record
		self.version_ = ProtorecordTypes_pb2.Version()

		# the parsed IndexSummary
		self.index_summary_ = ProtorecordIndex_pb2.IndexSummary()

		# the current parsed index item
		self.index_item_ = ProtorecordIndex_pb2.IndexItem()

		# the opened index file
		self.index_file_ = None

		# the opened data file
		self.data_file_ = None

		# buffer used to deserialize data from files
		self.buffer_ = []

		# the next item index the class will read from
		self.next_item_num_ = 0

		# set to true if any internal failure occurs
		self.failbit_ = False

		# set to a human reasble string explaing previous method's failure
		self.fail_reason_ = ""

		self.initialized_ = self._init_record(filepath)

	# Closes all opened file descriptors. This method is automatically
	# called by class's destructor.
	def close(self):
		if self.initialized_:
			self.index_file_.close()
			self.data_file_.close()

	def has_next(self):
		self.fail_reason_ = ""
		return self.initialized_ and not self.failbit_ and self.next_item_num_ < self.index_summary_.total_items()

	def get_next(self,ProtoMsg):
		okay = self.initialized_
		self.fail_reason_ = ""

		msg = ProtoMsg()
		okay = okay and self.has_next()
		okay = okay and self._get_index_item(self.next_item_num_)

		if okay:
			# TODO could probably optimize this out
			self.data_file_.seek(self.index_item_.offset)

			self.buffer_ = self.data_file_.read(self.index_item_.size)
			if self.buffer_ == '':
				self.fail_reason_ = "reached end of data file"
				okay = Flase

		if okay and not msg.ParseFromString(self.buffer_):
			self.fail_reason_ = "protobuf parse failed"
			okay = Flase

		if not okay:
			self.failbit_ = True

		return (msg,okay)

	def take_next(self,ProtoMsg):
		msg,okay = self.get_next(ProtoMsg)
		if okay:
			self.next_item_num_ += 1
		return (msg,okay)

	def size(self):
		self.fail_reason_ = ""
		if self.initialized_:
			return self.index_summary_.total_items
		return 0

	def flags(self):
		if self.initialized_:
			self.fail_reason_ = ""
			return self.index_summary_.flags()
		else:
			self.fail_reason_ = "Reader not initialized"
			return 0

	# @return
	# True if the record's HAS_ASSUMED_DATA flag is set, false otherwise.
	def has_assumed_data(self):
		self.fail_reason_ = ""
		return self._is_flag_set(PROTORECORD_FLAGS_HAS_ASSUMED_DATA)

	# @return
	# True if the record contains timestamped items, false otherwise.
	def has_timestamps(self):
		self.fail_reason_ = ""
		return self._is_flag_set(PROTORECORD_FLAGS_HAS_TIMESTAMPS)

	# @param[out] start_time_us
	# The records start time in microseconds
	#
	# @return
	# True on success, false otherwise. If false is returned, the value
	# of 'start_time_us' should not be assumed.
	def get_start_time(self):
		self.fail_reason_ = ""
		if not self.initialized_:
			self.fail_reason_ = "Reader not initialized"
		start_time_us = self.index_summary_.start_time_utc()
		return (start_time_us,self.initialized_)

	# @return
	# The protorecord library version that the record was made with.
	# This can be used to determine version incompatibilities. Returns
	# version 0.0.0 if Reader was not initialized properly.
	def get_version(self):
		self.fail_reason_ = ""
		if not self.initialized_:
			self.fail_reason_ = "Reader not initialized"
		return self.version_

	# @return
	# A string explaining the failure reason for a previously called
	# method in this class. An empty string is returned if the previous
	# method was successful. This method will also return an empty
	# string upon subsequent calls, in affect "popping" the failure
	# reason from the class.
	def reason(self):
		return self.fail_reason_

	# Initializes the class for read. This method is called from
	# the constructor and is used to setup the internal member
	# variables.
	#
	# @param[in] filepath
	# The record path to attempt to initial the class with
	#
	# @return
	# True if successfully initialized, false otherwise
	def _init_record(self, filepath):
		self.fail_reason_ = ""
		okay = True

		# open the index file
		INDEX_FLAGS = "rb"
		INDEX_FILEPATH = os.path.join(filepath,"index")
		if okay:
			try:
				self.index_file_ = open(INDEX_FILEPATH,INDEX_FLAGS)
			except IOError:
				self.fail_reason_ = "failed to open index file '%s'" % INDEX_FILEPATH
				okay = False

		# open the data file
		DATA_FLAGS = "rb"
		DATA_FILEPATH = os.path.join(filepath,"data")
		if okay:
			try:
				self.data_file_ = open(DATA_FILEPATH,DATA_FLAGS)
			except IOError:
				self.fail_reason_ = "failed to open data file '%s'" % DATA_FILEPATH
				okay = False

		# set version to invalid default
		self.version_.major = 0
		self.version_.minor = 0
		self.version_.patch = 0

		# intialize index item
		self.index_item_.offset = 0
		self.index_item_.size = 0

		# initialize index summary
		self.index_summary_.total_items = 0
		self.index_summary_.index_item_size = self.index_item_.ByteSize()
		self.index_summary_.start_time_utc = 0
		self.index_summary_.flags = 0

		try:
			# read library version from record
			if okay:
				self.buffer_ = self.index_file_.read(PROTORECORD_VERSION_SIZE)
				if self.buffer_ != '':
					self.version_.ParseFromString(self.buffer_)

					# check for compatibility
					if not self._is_compatible(self.version_):
						self.fail_reason_ = "file/library version incompatibility"
						okay = False
				else:
					self.fail_reason_ = "index file too small to parse version"
					okay = False

			# read IndexSummary from record
			if okay:
				self.buffer_ = self.index_file_.read(PROTORECORD_INDEX_SUMMARY_SIZE)

				if self.buffer_ != '':
					self.index_summary_.ParseFromString(self.buffer_)
				else:
					self.fail_reason_ = "index file too small to parse IndexSummary"
					okay = False
		except Exception as ex:
			self.fail_reason_ = "caught Exception: %s" % str(ex)
			okay = False

		return okay

	# @return
	# True if record's version number is compatible with this version of
	# Reader class.
	def _is_compatible(self, version):
		return True

	# Parse the next index item from the index_file_ and places the
	# result in the classes index_item_ member
	#
	# @param[in] item_idx
	# The index item to read from the index_file
	#
	# @param[out] item_out
	# The parsed IndexItem
	#
	# @return
	# True if index_item was parsed successfully, false otherwise
	def _get_index_item(self,item_idx):
		self.fail_reason_ = ""
		okay = self.initialized_ and item_idx < self.size()

		if okay:
			# compute position to IndexItem in file
			pos = PROTORECORD_VERSION_SIZE + PROTORECORD_INDEX_SUMMARY_SIZE
			pos += item_idx * self.index_summary_.index_item_size()

			# seek to position and read
			self.index_file_.seek(pos)
			self.buffer_ = self.index_file_.read(self.index_summary_.index_item_size())
			if self.buffer_ == '':
				self.fail_reason_ = "reached end of index file"
				okay = False
			else:
				if not self.index_item_.ParseFromString(self.buffer_):
					self.fail_reason_ = 'Failed to parse index item %d' % item_idx
					okay = False

		return okay

	# @param[in] flag
	# The flag to check for
	#
	# @return
	# True if the record flags are valid and the flag is set
	def _is_flag_set(self,flag):
		flags = self.flags()
		is_set = True
		is_set = is_set and (flags & PROTORECORD_FLAGS_VALID)
		is_set = is_set and (flags & flag)
		return is_set