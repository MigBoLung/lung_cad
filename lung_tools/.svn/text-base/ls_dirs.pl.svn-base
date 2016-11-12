#!perl

$base = $ARGV[0];

@dir_parent = $base;

while ( @dir_parent )
{
	$directory = shift (@dir_parent);
	opendir( DIR , $directory ) || next;

	while ( defined( $child = readdir(DIR) ) )
	{
		if ( (-d "$directory/$child") &&
		     ( $child ne "."  ) &&
		     ( $child ne ".." ) )
		{
      			push ( @dir_parent , "$directory/$child" );
			push ( @dir_list   , "$directory/$child" );
     		}
   	}

	closedir ( DIR );
}

$" = "\n";

if (@dir_list)
{
	print "@dir_list";
}

$" = " ";

exit;
