<?php

declare(strict_types=1);

$config_path = getenv('WP_CONFIG_PATH');
$db_name = getenv('WP_DB_NAME');
$db_user = getenv('WP_DB_USER');
$db_host = getenv('WP_DB_HOST');
$redis_host = getenv('WP_REDIS_HOST');
$redis_port = getenv('WP_REDIS_PORT');
$redis_password_file = getenv('WP_REDIS_PASSWORD_FILE');
$db_password = stream_get_contents(STDIN);

if (
	false === $config_path || '' === $config_path
	|| false === $db_name || '' === $db_name
	|| false === $db_user || '' === $db_user
	|| false === $db_host || '' === $db_host
	|| false === $redis_host || '' === $redis_host
	|| false === $redis_port || '' === $redis_port
	|| false === $redis_password_file || '' === $redis_password_file
	|| false === $db_password
) {
	fwrite(STDERR, "WordPress configuration values are incomplete.\n");
	exit(1);
}

$db_password = rtrim($db_password, "\r\n");
if ('' === $db_password) {
	fwrite(STDERR, "Database password is empty.\n");
	exit(1);
}

$redis_port = filter_var(
	$redis_port,
	FILTER_VALIDATE_INT,
	[
		'options' => [
			'min_range' => 1,
			'max_range' => 65535,
		],
	]
);
$redis_password = file_get_contents($redis_password_file);
if (false === $redis_port || false === $redis_password) {
	fwrite(STDERR, "Redis configuration values are incomplete.\n");
	exit(1);
}
$redis_password = rtrim($redis_password, "\r\n");
if ('' === $redis_password) {
	fwrite(STDERR, "Redis password is empty.\n");
	exit(1);
}

$config_values = [
	'DB_NAME' => $db_name,
	'DB_USER' => $db_user,
	'DB_PASSWORD' => $db_password,
	'DB_HOST' => $db_host,
	'DB_CHARSET' => 'utf8mb4',
	'DB_COLLATE' => '',
	'WP_REDIS_HOST' => $redis_host,
	'WP_REDIS_PORT' => $redis_port,
	'WP_REDIS_PASSWORD' => $redis_password,
];

$content = "<?php\n\n";
foreach ($config_values as $name => $value) {
	$content .= "define( " . var_export($name, true) . ', '
		. var_export($value, true) . " );\n";
}

$salt_names = [
	'AUTH_KEY',
	'SECURE_AUTH_KEY',
	'LOGGED_IN_KEY',
	'NONCE_KEY',
	'AUTH_SALT',
	'SECURE_AUTH_SALT',
	'LOGGED_IN_SALT',
	'NONCE_SALT',
];

try {
	foreach ($salt_names as $name) {
		$salt = bin2hex(random_bytes(64));
		$content .= "define( " . var_export($name, true) . ', '
			. var_export($salt, true) . " );\n";
	}
} catch (Throwable $exception) {
	fwrite(STDERR, "Unable to generate WordPress salts.\n");
	exit(1);
}

$content .= "\n\$table_prefix = 'wp_';\n";
$content .= "define( 'WP_DEBUG', false );\n";
$content .= "if ( ! defined( 'ABSPATH' ) ) { define( 'ABSPATH', __DIR__ . '/' ); }\n";
$content .= "require_once ABSPATH . 'wp-settings.php';\n";

if (false === file_put_contents($config_path, $content, LOCK_EX)) {
	fwrite(STDERR, "Unable to write wp-config.php.\n");
	exit(1);
}
