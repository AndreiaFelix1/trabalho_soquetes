#!/bin/bash

for i in {1..10}
do
    xterm -title "Cliente $i" -e "bash -c '
        {
            echo \"LOGIN andreia 1234\"

            sleep 2

            echo \"CREATE sala$i $((i * 100))\"

            sleep 2

            echo \"GET sala$i\"

            sleep 2

            echo \"LIST\"

            sleep 2

            echo \"SET sala$i $((i * 100 + 50))\"

            sleep 2

            echo \"GET sala$i\"

            sleep 2

            echo \"RESERVE sala$i\"

            # Mantém o cliente conectado
            while true
            do
                sleep 60
            done

        } | ./cliente

        exec bash
    '" &
done

wait