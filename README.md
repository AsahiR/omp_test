# 内容
openMpをつかってdiffusion問題とクイックソートを並列化した.
# 実行環境
tsubame3のqノード(4core)
# ディレクトリ構成
.
├── diffusion   
├── log  
├── plot  
└── sort  
* diffusion  
diffusion問題のソース+Makefile+ジョブスクリプトjob.sh
* sort  
マージソートのソース+Makefile+ジョブスクリプトjob.sh
* log  
出力結果.csvファイルが格納される.
* plot  
plot.pyでlogファイルを読み込んでplot図をeps形式で出力する.
# ビルド&実行方法
両者ともMakefileと同じ階層でmakeすると実行形式をget
* diffusion  
  * job.sh  
`qsub job.sh -t time_size -c core_size -i data_size` 
  * -t 実行形式に与える時間数.デフォルトは40  
  * -c スレッドサイズ.  デフォルトは4  
  * -i 行列の1辺のサイズを2の対数で指定.デフォルトは13  
  * 実行形式  
`diffusion_y -l -x column_size -y row_size -t time_step` 
    * -l 結果をoutput/以下に出力.答え合わせにつかえる.  
    * -y 行列の列サイズ.デフォルトは8192  
    * -t 時間数.デフォルトは40  
* sort  
  * job.sh
`qsub job.sh -c core_size -i data_size -l task_limit_size` 
  * -l 実行形式に渡す.デフォルトは1000
  * 実行形式  
`sort -n data_size -r rng_seed -l task_limit_size`  
  * -n ソートするデータのサイズ  
  * -r データの値につかう乱数のシード.シードが同じなら似たような結果.  
  * -l タスクを生成する最小のデータサイズ.タスク生成にはコストがかかるため制限をかける. 
  
# 実行結果  
* diffusion問題  
<figure>
<legend>データサイズ or スレッド数を変更した場合</legend>
<img src="http://art1.photozou.jp/pub/135/3222135/photo/256021393.v1527679970.png" 
</figure>  

* sort問題　　
<figure>
<legend>データサイズ or スレッド数を変更した場合</legend>
<img src="http://art1.photozou.jp/pub/135/3222135/photo/256021393.v1527679970.png">
</figure>
