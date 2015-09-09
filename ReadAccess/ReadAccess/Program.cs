using System;
using System.Collections.Generic;
using System.Data.OleDb;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ReadAccess
{
    class Program
    {
        static void Main(string[] args)
        {
            string connectionStr =
                @"Provider=Microsoft.Jet.OLEDB.4.0;Data Source = C:\Users\famg\Downloads\myDb.mdb";
            OleDbConnection conn = new OleDbConnection(connectionStr);
            string sqlStr = "select * from Senior0907";
            OleDbCommand command = new OleDbCommand(sqlStr, conn);

            List<string> TeacherName = new List<string>();
            Dictionary<string,int> TeacherDictionary = new Dictionary<string,int>();
            string tempName = "";
            try
            {
                conn.Open();
                Console.WriteLine("連線成功");
                OleDbDataReader dataReader = command.ExecuteReader();
                dataReader.Read(); dataReader.Read();
                while (dataReader.Read())
                {
                    int ColumnName = 2;
                    //Console.Write(dataReader["id"].ToString());
                    for (int i = ColumnName; i < ColumnName + 19; i++)
                    {
                        tempName = dataReader["欄位" + i].ToString();
                        //Console.Write(tempName + ",");
                        if (TeacherName.IndexOf(tempName) == -1)
                        {
                            TeacherName.Add(tempName);
                            TeacherDictionary.Add(tempName, 0);
                        }
                    }
                }
                dataReader.Close();

                //姓名比對
                dataReader = command.ExecuteReader();
                dataReader.Read(); dataReader.Read();
                while (dataReader.Read()) {
                    int ColumnName = 2;
                    for (int i = ColumnName; i < ColumnName + 19; i++)
                    {
                        tempName = dataReader["欄位" + i].ToString();
                        if (TeacherDictionary.ContainsKey(tempName))
                        {
                        TeacherDictionary[tempName] += 1;
                        }
                        tempName = "";
                    }
                }

                Console.WriteLine();
                dataReader.Close();
                conn.Close();
                Console.WriteLine("關閉連線");
            }
            catch (Exception ee)
            {
                Console.WriteLine("連線失敗{0}", ee);
            }
            Console.WriteLine("Total Teacher:{0}", TeacherName.Count);
            foreach (string name in TeacherDictionary.Keys)
            {
                Console.WriteLine(name + "," + TeacherDictionary[name]) ;
            }

            Console.ReadLine();
        }
    }
}
