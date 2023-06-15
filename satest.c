#include "smallarray.h"
#include <stdio.h>
#include <stdlib.h>




int main( int argc, char **argv ) {

    int testSize = 300;
    struct smallarray *smallarray = createSmallArray( testSize );
    
        
    for ( int i = 0; i < 300; i += 4 ) {
        setSmallArrayValue( smallarray, i );
    }

    for ( int i = 0; i < 300; i ++ ) {
        if ( getSmallArrayValue( smallarray, i ) ) {
            printf( "%i\n", i );
        }
    }



    int testIndex = 20;
    printf( "Before setting: %i\n", getSmallArrayValue( smallarray, testIndex ) );

    setSmallArrayValue( smallarray, testIndex );
    printf( "After setting: %i\n", getSmallArrayValue( smallarray, testIndex ) );






    return 0;
}

