NR=1

function CompareOutput
{
	DIFF=$(diff $1 $2)
	if [ "$DIFF" != "" ]
	then
		echo $DIFF
	else
		echo $1 "i" $2 "są takie same"
	fi
}

function Test
{
	echo "TEST" $NR
	((NR++))
	echo "Porównanie z" $1 "wczytujemy" $2 "bajtów"
	echo "Mój klient:"
	#time ./transport 40002 output $2 > moj.log
	/usr/bin/time -v ./transport 40002 output $2 > moj.log
	echo ""
	echo $1 ":"
	/usr/bin/time -v ./$1 40002 output2 $2 > ten_drugi.log
	echo ""
	echo "Sprawdzenie diffem:"
	CompareOutput output output2
	rm output output2
}


echo "PRACOWNIA SIECI KOMPUTEROWE ZADANIE 3"
echo ""

echo "KOMPILACJA"
echo "Czyszczenie:"
make distclean
echo "Budowanie"
make

#Test transport-faster 19000000
Test transport-slower 1337
#Test transport-slower 13337

