CREATE TABLE IF NOT EXISTS `chat_processor_filters` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `regex` varchar(255) COLLATE utf8_bin NOT NULL,
  `report_type` varchar(100) COLLATE utf8_bin NOT NULL,
  `report_count` int(11) NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

-- Dumping data for table wotlk_world.chat_processor_filters: ~0 rows (approximately)
/*!40000 ALTER TABLE `chat_processor_filters` DISABLE KEYS */;
REPLACE INTO `chat_processor_filters` (`id`, `regex`, `report_type`, `report_count`) VALUES
	(1, 'buy gold', 'GOLD SELLER', 1),
	(2, 'gold stock', 'GOLD SELLER', 1),
	(3, 'free gold', 'GOLD SELLER', 1),
	(4, 'cheap gold', 'GOLD SELLER', 1),
	(5, 'sell gold', 'GOLD SELLER', 1),
	(6, 'real money', 'GOLD SELLER', 1),
	(7, '(^|[^a-z]+)([u]+[^a-z]*[s]+[^a-z]*[d]+)($|[^a-z])', 'GOLD SELLER', 3),
	(8, '(^|[^a-z]+)([e]+[^a-z]*[u]+[^a-z]*[r]+)($|[^a-z])', 'GOLD SELLER', 3);
