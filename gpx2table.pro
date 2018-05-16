TEMPLATE = subdirs

SUBDIRS = \
	gpx2table \		# app
	GPX 			# lib from github.com/raptorswing/GPX

gpx2table.depends = GPX
