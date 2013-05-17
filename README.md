opdumper - A tiny and simple php extension that dumps opcodes of PHP script.

## VERSION
version: 0.0.1

## SUPPORT
PHP 5.3 / PHP 5.4 support

## INSTALL
    phpize
    ./configure
    make & make install

## API
### CLI API
Dump opcodes with command line: 

+ php -d opdumer.active=1 foo.php

### PHP FUNCTION API
Dump opcodes by PHP FUNCTION, both functions return an array that contains opcodes.

+ [ARRAY] od_dump_opcodes_string ([STRING] php_script) 
+ [ARRAY] od_dump_opcodes_file   ([STRING] filename) 
