for num  in 01 02 03 04 05
do
  for iter in 1 10 100
  do
    ./train $iter model_init.txt seq_model_$num.txt exp/model_$num\_$iter.txt
  done
done
