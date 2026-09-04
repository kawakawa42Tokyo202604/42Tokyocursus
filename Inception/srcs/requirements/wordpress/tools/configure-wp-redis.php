<?php

declare(strict_types=1);

$config_path = getenv('WP_CONFIG_PATH');
$output_path = getenv('WP_CONFIG_OUTPUT');
$redis_host = getenv('WP_REDIS_HOST');
$redis_port = getenv('WP_REDIS_PORT');
$redis_password_file = getenv('WP_REDIS_PASSWORD_FILE');

if (
	false === $config_path || '' === $config_path
	|| false === $output_path || '' === $output_path
	|| false === $redis_host || '' === $redis_host
	|| false === $redis_port || '' === $redis_port
	|| false === $redis_password_file || '' === $redis_password_file
) {
	fwrite(STDERR, "Redis configuration values are incomplete.\n");
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
$content = file_get_contents($config_path);
if (false === $redis_port || false === $redis_password || false === $content) {
	fwrite(STDERR, "Unable to read Redis configuration inputs.\n");
	exit(1);
}

$redis_password = rtrim($redis_password, "\r\n");
if ('' === $redis_password) {
	fwrite(STDERR, "Redis password is empty.\n");
	exit(1);
}

$marker = "require_once ABSPATH . 'wp-settings.php';";
$position = strrpos($content, $marker);
if (false === $position) {
	fwrite(STDERR, "Unable to find the WordPress bootstrap marker.\n");
	exit(1);
}

$definitions = '';
foreach (
	[
		'WP_REDIS_HOST' => $redis_host,
		'WP_REDIS_PORT' => $redis_port,
		'WP_REDIS_PASSWORD' => $redis_password,
	] as $name => $value
) {
	$definitions .= "define( " . var_export($name, true) . ', '
		. var_export($value, true) . " );\n";
}

$updated_content = substr_replace($content, $definitions, $position, 0);
if (false === file_put_contents($output_path, $updated_content, LOCK_EX)) {
	fwrite(STDERR, "Unable to update wp-config.php.\n");
	exit(1);
}
