using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data;
using System.Data.SqlClient;

namespace ConsoleApplication4_LINQ
{
    class Program
    {
        static void Main(string[] args)
        {
            //int switcher = int.Parse(Console.ReadLine());
            int switcher = 4;
            switch (switcher)
            {
                case 1:
                    Console.WriteLine("tutorial 1\n=========================================\n");
                    Linq_A P = new Linq_A();
                    P.display();

                    break;
                case 2:
                    #region C#2.0
                    int[] source = new int[] { 0, -5, 12, -54, 5, -67, 3, 6 };
                    List<int> results = new List<int>();
                    foreach (int integer in source)
                    {
                        if (integer > 0)
                            results.Add(integer);
                    }
                    //不知道在幹嘛，似乎是做排序的...
                    Comparison<int> comparsion = delegate(int a, int b)
                    {
                        return a - b;
                    };
                    results.Sort(comparsion);
                    foreach (int item in results)
                    {
                        Console.WriteLine(item);
                    }
                    #endregion
                    break;
                case 3:
                    #region C#3.0 改善排序方法(類似T-SQL)
                    int[] source2 = new int[] { 0, -5, 12, -54, 5, -67, 3, 6 };
                    var results2 = from integer in source2
                                   where integer > 0
                                   orderby integer descending
                                   select integer;
                    foreach (int i in results2)
                    {
                        Console.WriteLine(i);
                    }
                    #endregion
                    break;
                case 4:
                    ///
                    ///從資料庫讀出字串並且使用 dictionary 存取
                    ///
                    #region C#2.0
                    Dictionary<string, string> results3 = new Dictionary<string, string>();
                    //設定連線字串
                    using (SqlConnection connection = new SqlConnection(@"Data Source =localhost;Initial Catalog = AdventureWorks2012;Integrated Security = True "))
                    //sql語法
                    using (SqlCommand command = new SqlCommand(@"SELECT TOP 1000 [SalesReasonID],[Name],[ReasonType],[ModifiedDate] FROM [AdventureWorks2012].[Sales].[SalesReason]", connection))
                    {
                        //開啟連線
                        connection.Open();
                        using (SqlDataReader reader = command.ExecuteReader())
                        {
                            while (reader.Read())
                            {
                                string Name = (string)reader["Name"];
                                string ReasonType = (string)reader["ReasonType"];
                                results3.Add(Name, ReasonType); //加入dictionary內
                            }
                        }

                    }
                    //顯示出來
                    foreach (KeyValuePair<string, string> item in results3)
                    {
                        Console.WriteLine("{0}:{1}", item.Key, item.Value);
                    }
                    #endregion
                    break;

                case 5:



                    break;
                default:
                    Console.WriteLine("Please enter mode!!");
                    break;
            }
            //Example 1

            Console.Read();
        }

    }




}
